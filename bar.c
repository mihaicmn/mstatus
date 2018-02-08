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
	int i, sleep = 0, rc = 0;
	bool update;
	struct timespec time;
	struct block_t *b;

	MUTEX_LOCK;

	while (looping) {
		for (i = 0, b = blocks; i < block_count; i++, b++) {
			if (rc != ETIMEDOUT || sleep % b->interval == 0) {
				block_reload(b);
				update = true;
			}
		}

		clock_gettime(CLOCK_REALTIME, &time);
		time.tv_sec += sleeping_period;

		if (update && pthread_cond_signal(&update_cond) < 0)
			die("could not signal condition\n");

		if ((rc = pthread_cond_timedwait(&sleep_cond, &mutex, &time)) < 0)
			die("could not wait on condition\n");

		sleep += sleeping_period;
		sleep %= sleeping_cycle;
		update = false;
	}

	MUTEX_UNLOCK;
	return NULL;
}

void bar_loop() {
	block_count = config_get_block_count();
	blocks = smalloc(block_count * sizeof(struct block_t));

	cfg_t *general = config_get_general();
	output_init(general);

	int i;
	struct block_t *b;

	for (i = 0, b = blocks; i < block_count; i++, b++) {
		block_init(b, config_get_block(i));

		if (b->interval < 1) {
			b->interval = cfg_getint(general, "interval");
		}

		sleeping_period = (i == 0) ? b->interval : greatest_common_divisor(b->interval, sleeping_period);
		sleeping_cycle  = (i == 0) ? b->interval : b->interval * sleeping_cycle / sleeping_period;
	}

	if (pthread_mutex_init(&mutex, NULL) < 0)
		die("could not init mutex\n");
	if (pthread_cond_init(&update_cond, NULL) < 0)
		die("could not init update_cond\n");
	if (pthread_cond_init(&sleep_cond, NULL) < 0)
		die("could not init sleep_cond\n");
	if (pthread_create(&thread, NULL, start_polling, NULL) < 0)
                die("could not create polling thread\n");

	MUTEX_LOCK;

	while (looping) {
		output_begin();

		for (i = 0, b = blocks; i < block_count; i++, b++) {
			block_render(b, &output_print);
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
	if (pthread_cond_destroy(&update_cond) < 0)
		die("could not destroy update_cond\n");
	if (pthread_cond_destroy(&sleep_cond) < 0)
		die("could not destroy sleep_cond\n");
	if (pthread_mutex_destroy(&mutex) < 0)
		die("cound not destroy mutex\n");

	for (i = 0, b = blocks; i < block_count; i++, b++) {
		block_dismiss(b);
	}
	sfree(blocks);

	output_dismiss();
}

void bar_kill() {
	MUTEX_LOCK;
	looping = false;
	if (pthread_cond_signal(&update_cond) < 0)
		die("could not signal condition\n");
	MUTEX_UNLOCK;
}

void bar_refresh() {
	MUTEX_LOCK;
	if (pthread_cond_signal(&sleep_cond) < 0)
		die("could not signal condition\n");
	MUTEX_UNLOCK;
}
