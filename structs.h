#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#define ELEMENTOS 50

typedef struct {
    char name[50];
    int prepTime;
}Food;

typedef struct {
    int inicio, fin;
    Food buf[ELEMENTOS];
    sem_t *lleno, *vacio, *leyendo, *escribiendo;
} boundedBuffer_t;

typedef struct{
    boundedBuffer_t lista_pedidos;
    boundedBuffer_t lista_terminados;
}Monitor_t;

typedef struct {
    int id;
    int tolerance;
    Food *order;
    sem_t *semQueue;
    pthread_mutex_t *mtx;
}Client;

// typedef struct {
//     int id;
//     int ocupado;
//     sem_t *semServed;
//     sem_t *semCook;
//     sem_t *semCheckout;
//     sem_t *semQueue;
//     Client *client;
// }Cashier;

typedef struct {
    Food *menu;
    Client *clients;
    Food *currentOrder;
    int open;
    sem_t *semQueue;
    pthread_mutex_t *mtx;

}FoodPlace;

typedef struct {
    int id;
    int *libre;
    Monitor_t *m;   
    pthread_mutex_t *mtx; 
}Chef;


#endif