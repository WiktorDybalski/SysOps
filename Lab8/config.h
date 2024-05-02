
#define QUEUE_LEN  10
#define PRINT_SIZE  10
#define SHARED_MEMORY "shared_memory"


typedef struct {
    char jobs[QUEUE_LEN][PRINT_SIZE];
    int put_index;
    int take_index;
} PrintQueue;


