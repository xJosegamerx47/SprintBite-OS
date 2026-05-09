#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TOTAL_ORDERS 30
#define NUM_COOKS 5
#define INITIAL_INVENTORY 72

int order_counter = 1;
int processed_orders = 0;
int inventory = INITIAL_INVENTORY;
int race_indicators = 0;
int negative_inventory_events = 0;

void *cook_worker(void *arg) {
    int cook_id = *(int *)arg;

    while (1) {
        if (order_counter > TOTAL_ORDERS) {
            break;
        }

        int order_id = order_counter;
        usleep(1000);
        order_counter++;

        int consumed = 2 + (order_id % 4);
        int read_inventory = inventory;

        usleep(1500);

        inventory = read_inventory - consumed;
        processed_orders++;

        if (order_id != processed_orders) {
            race_indicators++;
        }

        printf("[UNSAFE][Cook %d] order #%d | read inventory=%d | consumed=%d | now=%d\n",
               cook_id, order_id, read_inventory, consumed, inventory);

        if (inventory < 0) {
            negative_inventory_events++;
            printf("[UNSAFE][Cook %d][WARNING] inventory became negative after order #%d\n",
                   cook_id, order_id);
        }

        usleep(1000);
    }

    return NULL;
}

int main() {
    pthread_t cooks[NUM_COOKS];
    int cook_ids[NUM_COOKS];

    printf("============================================================\n");
    printf(" SprintBite OS - UNSAFE module\n");
    printf(" Race condition demo without mutexes or semaphores\n");
    printf(" Orders=%d | Cooks=%d | Initial inventory=%d\n", TOTAL_ORDERS, NUM_COOKS, INITIAL_INVENTORY);
    printf("============================================================\n");

    for (int i = 0; i < NUM_COOKS; i++) {
        cook_ids[i] = i + 1;
        pthread_create(&cooks[i], NULL, cook_worker, &cook_ids[i]);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cooks[i], NULL);
    }

    printf("----------------------------- SUMMARY -----------------------\n");
    printf("Requested orders:              %d\n", TOTAL_ORDERS);
    printf("Processed orders reported:     %d\n", processed_orders);
    printf("Final order counter:           %d\n", order_counter);
    printf("Race-condition indicators:     %d\n", race_indicators);
    printf("Negative inventory events:     %d\n", negative_inventory_events);
    printf("Final inventory value:         %d\n", inventory);
    printf("Synchronization status:        INCONSISTENT\n");
    printf("-------------------------------------------------------------\n");

    return 0;
}
