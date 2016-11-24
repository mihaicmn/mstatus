
struct item_t {
	int interval;
	char *message;
};

void item_refresh(struct item_t *item);
void item_print(struct item_t *item);
void item_destroy(struct item_t *item);
