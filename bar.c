#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#include "bar.h"
#include "block.h"
#include "output.h"
#include "config.h"
#include "util.h"

#define FOREACH_BLOCK struct block_t *block; int i; for (i = 0, block = blocks; i < block_count; i++, block++)

static pthread_t thread;
static pthread_mutex_t mutex;
static pthread_cond_t condition;

struct block_t *blocks;
int block_count;

static int sleeping_period;
static int sleeping_cycle;


static int greatest_common_divisor(int x, int y) {
	while(x != y)
		(x > y) ? (x -= y) : (y -= x);
	return x;
}

static void *start_polling(void *arg) {
	int sleeping_duration = 0;
	bool should_trigger_update;

	while (1) {
		if (pthread_mutex_lock(&mutex) < 0)
			die("could not lock mutex\n");

		FOREACH_BLOCK {
			if (sleeping_duration % block->interval == 0) {
				block_refresh(block);
				should_trigger_update = true;
			}
		}

		if (should_trigger_update && pthread_cond_signal(&condition) < 0)
			die("could not signal on condition\n");
		if (pthread_mutex_unlock(&mutex) < 0)
			die("could not unlock mutex\n");

		sleep(sleeping_period);
		sleeping_duration += sleeping_period;
		sleeping_duration %= sleeping_cycle;
		should_trigger_update = false;
	}
}

void bar_init() {
	int general_interval;

	if (pthread_mutex_init(&mutex, NULL) < 0)
		die("could not initialize mutex\n");
	if (pthread_cond_init(&condition, NULL) < 0)
		die("could not initialize contition\n");

	block_count = config_get_block_count();
	blocks = smalloc(block_count * sizeof(struct block_t));

	cfg_t *general = config_get_general();
	general_interval = cfg_getint(general, "interval");
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
}

void bar_loop() {
	if (pthread_create(&thread, NULL, start_polling, NULL) < 0)
                die("could not create polling thread\n");

	while (1) {
		if (pthread_mutex_lock(&mutex) < 0)
			die("could not lock mutex\n");
		if (pthread_cond_wait(&condition, &mutex) < 0)
			die("could not wait on condition\n");

		output_begin();

		FOREACH_BLOCK {
			block_print(block);
		}

		output_end();

		if (pthread_mutex_unlock(&mutex) < 0)
			die("could not unlock mutex\n");
	}
}

void bar_dismiss() {
	if (pthread_cancel(thread) < 0)
		die("could not cancel polling thread\n");
	if (pthread_join(thread, NULL) < 0)
		die("could not join polling thread\n");
	if (pthread_mutex_destroy(&mutex) < 0)
		die("could not destroy mutex\n");
	if (pthread_cond_destroy(&condition) < 0)
		die("could not destroy condition\n");

	output_dismiss();

	FOREACH_BLOCK {
		block_clear(block);
	}
	
	sfree(blocks);
}
