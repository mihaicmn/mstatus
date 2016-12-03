#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#include "bar.h"
#include "config.h"
#include "util.h"


static pthread_t thread;
static pthread_mutex_t mutex;
static pthread_cond_t condition;

struct item_t *items;
int item_count;


static int sleeping_period;
static int sleeping_cycle;


static int greatest_common_divisor(int x, int y) {
	while(x != y)
		(x > y) ? (x -= y) : (y -= x);
	return x;
}

static void *start_polling(void *arg) {
	struct item_t *item;
	int i, sleeping_duration = 0;
	bool should_trigger_update;

	while (1) {
		if (pthread_mutex_lock(&mutex) < 0)
			die("could not lock mutex\n");

		for (i = 0; i < item_count; i++) {
			item = items + i;
			if (sleeping_duration % item->interval == 0) {
				item_refresh(item);
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
	struct item_t *item;
	int i, general_interval;

	if (pthread_mutex_init(&mutex, NULL) < 0)
		die("could not initialize mutex\n");
	if (pthread_cond_init(&condition, NULL) < 0)
		die("could not initialize contition\n");

	item_count = config_get_item_count();
	items = smalloc(item_count * sizeof(struct item_t));

	cfg_t *general = config_get_general();
	general_interval = cfg_getint(general, "interval");
	output_init(general);

	for (i = 0; i < item_count; i++) {
		item = items + i;
		item_init(item, config_get_item(i));
		if (item->interval < 1)
			item->interval = general_interval;
	}

	sleeping_period = items[0].interval;
	sleeping_cycle = items[0].interval;
	for (i = 1; i < item_count; i++) {
		item = items + i;
		sleeping_period = greatest_common_divisor(item->interval, sleeping_period);
		sleeping_cycle = item->interval * sleeping_cycle / sleeping_period;
	}
}

void bar_loop() {
	int i;
	if (pthread_create(&thread, NULL, start_polling, NULL) < 0)
                die("could not create polling thread\n");

	while (1) {
		if (pthread_mutex_lock(&mutex) < 0)
			die("could not lock mutex\n");

		if (pthread_cond_wait(&condition, &mutex) < 0)
			die("could not wait on condition\n");

		output_begin();
		for (i = 0; i < item_count; i++)
			output_print(&items[i].text);
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

	int i;
	for (i = 0; i < item_count; i++)
		item_clear(items + i);
	
	sfree(items);
}
