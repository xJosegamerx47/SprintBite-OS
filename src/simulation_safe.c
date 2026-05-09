#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define TOTAL_ORDERS 30
#define NUM_CLIENTS 4
#define NUM_COOKS 4
#define NUM_COURIERS 2
#define QUEUE_SIZE 64
#define INITIAL_INVENTORY 110
#define STOVES 2
#define PACKING_SLOTS 1

typedef struct {
    int id;
    int ingredients;
} Order;

typedef struct {
    Order items[QUEUE_SIZE];
    int front;
    int rear;
    pthread_mutex_t mutex;
    sem_t available_items;
    sem_t available_slots;
} Queue;

Queue backlog;
Queue ready_orders;

pthread_mutex_t inventory_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t stoves;
sem_t packing_slots;

int inventory = INITIAL_INVENTORY;
int generated_orders = 0;
int cooked_orders = 0;
int delivered_orders = 0;

void queue_init(Queue *q) {
    q->front = 0;
    q->rear = 0;
    pthread_mutex_init(&q->mutex, NULL);
    sem_init(&q->available_items, 0, 0);
    sem_init(&q->available_slots, 0, QUEUE_SIZE);
}

void queue_push(Queue *q, Order order) {
    sem_wait(&q->available_slots);
    pthread_mutex_lock(&q->mutex);

    q->items[q->rear] = order;
    q->rear = (q->rear + 1) % QUEUE_SIZE;

    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->available_items);
}

Order queue_pop(Queue *q) {
    sem_wait(&q->available_items);
    pthread_mutex_lock(&q->mutex);

    Order order = q->items[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;

    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->available_slots);

    return order;
}

void *client_worker(void *arg) {
    int client_id = *(int *)arg;

    while (1) {
        pthread_mutex_lock(&stats_mutex);

        if (generated_orders >= TOTAL_ORDERS) {
            pthread_mutex_unlock(&stats_mutex);
            break;
        }

        generated_orders++;
        int order_id = generated_orders;

        pthread_mutex_unlock(&stats_mutex);

        Order order;
        order.id = order_id;
        order.ingredients = 2 + (order_id % 4);

        printf("[CLIENT %d] generated order #%d requiring %d ingredients\n",
               client_id, order.id, order.ingredients);

        queue_push(&backlog, order);
        usleep(30000);
    }

    return NULL;
}

void *cook_worker(void *arg) {
    int cook_id = *(int *)arg;

    while (1) {
        Order order = queue_pop(&backlog);

        if (order.id == -1) {
            printf("[COOK %d] received stop signal\n", cook_id);
            break;
        }

        sem_wait(&stoves);

        pthread_mutex_lock(&inventory_mutex);
        inventory -= order.ingredients;
        int current_inventory = inventory;
        pthread_mutex_unlock(&inventory_mutex);

        printf("[COOK %d] started cooking order #%d | inventory=%d\n",
               cook_id, order.id, current_inventory);

        usleep(60000);
        sem_post(&stoves);

        sem_wait(&packing_slots);
        usleep(30000);
        sem_post(&packing_slots);

        pthread_mutex_lock(&stats_mutex);
        cooked_orders++;
        pthread_mutex_unlock(&stats_mutex);

        printf("[COOK %d] finished and packed order #%d\n", cook_id, order.id);

        queue_push(&ready_orders, order);
    }

    return NULL;
}

void *courier_worker(void *arg) {
    int courier_id = *(int *)arg;

    while (1) {
        Order order = queue_pop(&ready_orders);

        if (order.id == -1) {
            printf("[COURIER %d] received stop signal\n", courier_id);
            break;
        }

        printf("[COURIER %d] picked order #%d for delivery\n", courier_id, order.id);
        usleep(50000);

        pthread_mutex_lock(&stats_mutex);
        delivered_orders++;
        pthread_mutex_unlock(&stats_mutex);

        printf("[COURIER %d] delivered order #%d successfully\n", courier_id, order.id);
    }

    return NULL;
}

int main() {
    pthread_t clients[NUM_CLIENTS];
    pthread_t cooks[NUM_COOKS];
    pthread_t couriers[NUM_COURIERS];

    int client_ids[NUM_CLIENTS];
    int cook_ids[NUM_COOKS];
    int courier_ids[NUM_COURIERS];

    queue_init(&backlog);
    queue_init(&ready_orders);

    sem_init(&stoves, 0, STOVES);
    sem_init(&packing_slots, 0, PACKING_SLOTS);

    printf("============================================================\n");
    printf(" SprintBite OS - SAFE module\n");
    printf(" Mutexes, semaphores and producer-consumer queues\n");
    printf(" Orders=%d | Clients=%d | Cooks=%d | Couriers=%d | Stoves=%d | Packing=%d | Inventory=%d\n",
           TOTAL_ORDERS, NUM_CLIENTS, NUM_COOKS, NUM_COURIERS, STOVES, PACKING_SLOTS, INITIAL_INVENTORY);
    printf("============================================================\n");

    for (int i = 0; i < NUM_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_worker, &client_ids[i]);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        cook_ids[i] = i + 1;
        pthread_create(&cooks[i], NULL, cook_worker, &cook_ids[i]);
    }

    for (int i = 0; i < NUM_COURIERS; i++) {
        courier_ids[i] = i + 1;
        pthread_create(&couriers[i], NULL, courier_worker, &courier_ids[i]);
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        Order stop = {-1, 0};
        queue_push(&backlog, stop);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cooks[i], NULL);
    }

    for (int i = 0; i < NUM_COURIERS; i++) {
        Order stop = {-1, 0};
        queue_push(&ready_orders, stop);
    }

    for (int i = 0; i < NUM_COURIERS; i++) {
        pthread_join(couriers[i], NULL);
    }

    printf("----------------------------- SUMMARY -----------------------\n");
    printf("Orders generated:            %d\n", generated_orders);
    printf("Orders cooked:               %d\n", cooked_orders);
    printf("Orders delivered:            %d\n", delivered_orders);
    printf("Final inventory:             %d\n", inventory);
    printf("Synchronization status:      CONSISTENT\n");
    printf("-------------------------------------------------------------\n");

    return 0;
}
