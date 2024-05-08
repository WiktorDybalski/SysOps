#define PRINT_SIZE  10
#define MAX_PRINTERS 10
#define SHARED_MEMORY "shared_memory"

typedef enum {
    WAITING = 0,
    PRINTING = 1
} printer_state_t;

typedef struct {
    sem_t printer_semaphore;
    char printer_buffer[PRINT_SIZE + 1];
    size_t printer_buffer_size;
    printer_state_t printer_state;
} printer_t;

typedef struct {
    printer_t printers[MAX_PRINTERS];
    int number_of_printers;
} PrintQueue;
