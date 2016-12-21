#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "bar.h"
#include "block.h"
#include "output.h"
#include "config.h"
#include "util.h"

#define MUTEX_LOCK if (pthread_mutex_lock(&mutex) < 0) die("could not lock mutex\n")
#define MUTEX_UNLOCK if (pthread_mutex_unlock(&mutex) < 0) die("could not unlock mutex\n")

#define CONDITION_INIT(condition) if (pthread_cond_init(condition, NULL) < 0) die("could not init cond\n")
#define CONDITION_DSTR(condition) if (pthread_cond_destroy(condition) < 0) die("could not destroy cond\n")
#define CONDITION_SIGNAL(condition) if (pthread_cond_signal(condition) < 0) die("could not signal condition\n")

#define FOREACH_BLOCK struct block_t *block; int i; for (i = 0, block = blocks; i < block_count; i++, block++)

static pthread_t thread;
static pthread_mutex_t mutex;
static pthread_cond_t update_cond;
static pthread_cond_t sleep_cond;

static struct block_t *blocks;
static int block_count;

static int sleeping_period;
static int sleeping_cycle;

static bool looping = true;

static inline int greatest_common_divisor(int x, int y) {
	while(x != y)
		(x > y) ? (x -= y) : (y -= x);
	return x;
}

static void *start_polling(void *arg) {
	int sleeping_duration = 0, rc = 0;
	bool should_trigger_update;
	struct timespec time;

	MUTEX_LOCK;

	while (looping) {
		FOREACH_BLOCK {
			if (rc != ETIMEDOUT || sleeping_duration % block->interval == 0) {
				block_refresh(block);
				should_trigger_update = true;
			}
		}

		if (should_trigger_update)
			CONDITION_SIGNAL(&update_cond);

		clock_gettime(CLOCK_REALTIME, &time);
		time.tv_sec += sleeping_period;

		if ((rc = pthread_cond_timedwait(&sleep_cond, &mutex, &time)) < 0)
			die("could not wait on condition\n");

		sleeping_duration += sleeping_period;
		sleeping_duration %= sleeping_cycle;
		should_trigger_update = false;
	}

	MUTEX_UNLOCK;
}

void bar_init() {
	block_count = config_get_block_count();
	blocks = smalloc(block_count * sizeof(struct block_t));

	cfg_t *general = config_get_general();
	const int general_interval = cfg_getint(general, "interval");
	output_init(general);

	FOREACH_BLOCK {
		block_init(block, config_get_block(i));
		if (block->interval < 1)
			block->interval = general_interval;
	}

	sleeping_period = blocks[0].interval;
	sleeping_cycle = blocks[0].interval;

	for (i = 1; i < block_count; i++) {
		block = blocks + i;
		sleeping_period = greatest_common_divisor(block->interval, sleeping_period);
		sleeping_cycle = block->interval * sleeping_cycle / sleeping_period;
	}

	if (pthread_mutex_init(&mutex, NULL) < 0)
		die("could not init mutex\n");

	CONDITION_INIT(&update_cond);
	CONDITION_INIT(&sleep_cond);
}

void bar_dismiss() {
	output_dismiss();

	FOREACH_BLOCK {
		block_clear(block);
	}

	sfree(blocks);

	if (pthread_mutex_destroy(&mutex) < 0)
		die("cound not destroy mutex\n");

	CONDITION_DSTR(&update_cond);
	CONDITION_DSTR(&sleep_cond);
}

void bar_loop() {
	if (pthread_create(&thread, NULL, start_polling, NULL) < 0)
                die("could not create polling thread\n");

	MUTEX_LOCK;

	while (looping) {
		output_begin();

		FOREACH_BLOCK {
			block_print(block);
		}

		output_end();

		if (pthread_cond_wait(&update_cond, &mutex) < 0)
			die("could not wait on condition\n");
	}

	MUTEX_UNLOCK;

	if (pthread_cancel(thread) < 0)
		die("could not cancel polling thread\n");
	if (pthread_join(thread, NULL) < 0)
		die("could not join polling thread\n");
}

void bar_break() {
	MUTEX_LOCK;
	looping = false;
	CONDITION_SIGNAL(&update_cond);
	MUTEX_UNLOCK;
}

void bar_refresh() {
	MUTEX_LOCK;
	CONDITION_SIGNAL(&sleep_cond);
	MUTEX_UNLOCK;
}
