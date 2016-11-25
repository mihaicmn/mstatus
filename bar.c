#include <pthread.h>
#include <stdio.h>

#include "bar.h"
#include "config.h"
#include "util.h"


static pthread_t thread;
static pthread_mutex_t mutex;
static pthread_cond_t condition;


static void *start_polling(void *arg) {
	struct bar_t *bar = arg;
	int i;

	while (1) {
		if (pthread_mutex_lock(&mutex) < 0)
			die("could not lock mutex\n");

		for (i = 0; i < bar->count; i++)
			item_refresh(&bar->items[i]);

		if (pthread_cond_signal(&condition) < 0)
			die("could not signal on condition\n");

		if (pthread_mutex_unlock(&mutex) < 0)
			die("could not unlock mutex\n");

		sleep(1);
	}
}

struct bar_t *bar_create() {
	struct bar_t *bar = smalloc(sizeof(struct bar_t));
	if (pthread_mutex_init(&mutex, NULL) < 0)
		die("could not initialize mutex\n");
	if (pthread_cond_init(&condition, NULL) < 0)
		die("could not initialize contition\n");

	bar->count = config_get_item_count();
	bar->items = smalloc(bar->count * sizeof(struct item_t));

	int i;
	for (i = 0; i < bar->count; i++)
		item_init(&bar->items[i], config_get_item(i));
 
	return bar;
}

void bar_loop(struct bar_t *bar) {
	int i;
	if (pthread_create(&thread, NULL, start_polling, bar) < 0)
                die("could not create polling thread\n");

	while (1) {
		if (pthread_mutex_lock(&mutex) < 0)
			die("could not lock mutex\n");

		if (pthread_cond_wait(&condition, &mutex) < 0)
			die("could not wait on condition\n");

		for (i = 0; i < bar->count; i++)
			item_print(&bar->items[i]);
		printf("\n");


		if (pthread_mutex_unlock(&mutex) < 0)
			die("could not unlock mutex\n");
	}
}

void bar_destroy(struct bar_t *bar) {
	if (pthread_cancel(thread) < 0)
		die("could not cancel polling thread\n");
	if (pthread_join(thread, NULL) < 0)
		die("could not join polling thread\n");
	if (pthread_mutex_destroy(&mutex) < 0)
		die("could not destroy mutex\n");
	if (pthread_cond_destroy(&condition) < 0)
		die("could not destroy condition\n");

	int i;
	for (i = 0; i < bar->count; i++)
		item_clear(&bar->items[i]);
	
	sfree(bar->items);
	sfree(bar);
}
