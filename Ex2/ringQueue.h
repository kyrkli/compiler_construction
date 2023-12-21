#define RINGQ_SIZE 10000


void ringQueue_init();

void ringQueue_deinit();

void enqueue(char* el);

char* dequeue();

int is_empty();

void print_queue();
