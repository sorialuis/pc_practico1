#ifndef _STRUCTS_H_
#define _STRUCTS_H_

typedef struct {
    char name[50];
    int prepTime;
}Food;

typedef struct {
    int id;
    int tolerance;
    Food *order;
    sem_t *semQueue;
}Client;

typedef struct {
    int id;
    int ocupado;
    sem_t *semServed;
    sem_t *semCook;
    sem_t *semCheckout;
    sem_t *semQueue;
    Client *client;
}Cashier;

typedef struct {
    Food *menu;
    Cashier *cashier;
    Client *clients;
    Food *currentOrder;
    int open;
    sem_t *semQueue;
    
}FoodPlace;

typedef struct {
    Client *client;
    FoodPlace *mercadoChino;
}ClientOrder;

#endif