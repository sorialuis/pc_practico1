#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include "monitorBufferCircular.h"

#define COCINEROS 3

/* Variable global para la alarma */
int placeOpen, finished;


typedef struct {
    int *cooks; /*Cambiar por struct de cocineros*/
    int *queued_clients; /*Cambiar por struct de clientes con timeout y lo que pidieron*/
    int *finished_orders;

    FoodPlace *mercadoChino;
    pthread_t sThread;
}MenuParameters;

/* Thread functions */
void *streetThread(void *);
void *clientThread(void *);
void *chefThread(void *);

/* UserMenu Functions */
void *showMenu(void *);
void clearScreen();

/* UserMenu Options */
void startGame(FoodPlace *, pthread_t);
void serveClient();
void deliverOrder();
void closeFoodPlace();

/* FoodMenu Functions */
Food *menuSetup();
Food *pickFood(Food * menu);
int getMaxWaitTime(Food *menu);

/* Alarm */
void closeDoor(int);


int main() {
    pthread_t menu;
    int error;

    int cooks;
    int queued_clients;
    int finished_orders;    
    int opt;


    srand(time(NULL));

    cooks = 3;
    queued_clients = 5;
    placeOpen = 1;
    finished = 0;
    finished_orders = 3;


    MenuParameters *params = (MenuParameters*)calloc(1,sizeof(MenuParameters));
    params->mercadoChino = (FoodPlace *)calloc(1,sizeof(FoodPlace));
    params->mercadoChino->menu = menuSetup();
    params->cooks = &cooks;
    params->queued_clients = &queued_clients;
    params->finished_orders = &finished_orders;
    pthread_mutex_init(params->mercadoChino->mtx,NULL);
    pthread_mutex_lock(params->mercadoChino->mtx);

    params->mercadoChino->semQueue = sem_open("/semQueue", O_CREAT, O_RDWR, 3);

    if(params->mercadoChino->semQueue == SEM_FAILED) {
        perror("/semQueue");
        error = -1;
        return error;
    }

    pthread_create(&menu,NULL,showMenu,(void *)params);

    while (!finished){

        scanf("%d",&opt);
        /*El switch es feo... lo cambiamos ?*/

        switch (opt) {
            case 1:
                startGame(params->mercadoChino,params->sThread);
                break;

            case 2:
                serveClient();
                break;

            case 3:
                deliverOrder();
                break;

            case 4:
                closeFoodPlace();
                break;

            default:
                printf("OPCION INCORRECTA, NO SEAS MANCO");
        }

        fflush(stdin);
    }

    pthread_join(menu,NULL);

    return 1;
}

void *showMenu(void *args){
    MenuParameters *params;
    int i;
    char *status[2];

    params = (MenuParameters *)args;

    status[0] = "Ocupado";
    status[1] = "Desocupado";

    while (!finished){
        clearScreen();

        printf("Puerta: %s\n\n", placeOpen ? "ABIERTA" : "CERRADA");

        printf("Estados de Cocineros:\n");
        for(i = 1; i <= *params->cooks; i++ ){
            printf("\tCocinero %d: %s\n",i,status[rand()%2]);
        }

        printf("\nClientes en cola:\n");
        for(i = 1; i <= *params->queued_clients; i++ ){
            printf("\tCliente %d pedido %s se va en %d segundos\n",i,"%food_name%",10);
        }

        printf("\nPedidos Terminados: %d\n",*params->finished_orders);

        printf("\nAcciones:\n");
        printf("\t1 - Iniciar Juego\n");
        printf("\t2 - Atender Cliente\n");
        printf("\t3 - Entregar Pedido\n");
        printf("\t4 - Cerrar Local\n");
        printf("\nIngrese una opcion: \n");

        sleep(2);
    }

    pthread_exit(NULL);
}

void clearScreen()
{
    system("@cls||clear");
}

void startGame(FoodPlace *mercadoChino, pthread_t sThread){
    /*Si el juego no esta iniciado, Ejecutar la calle*/

    printf("\nJuego Iniciado\n");
    pthread_create(&sThread,NULL,streetThread,(void *)mercadoChino);

    /* Setear alarma de fin */
    signal(SIGALRM, closeDoor);
    alarm(10);

}

void serveClient(){

    /*Checkear si el juego esta iniciado.*/
    /*Checkear si hay clientes en la cola.*/
    /*Checkear si hay cocineros disponibles. */

    /*Asignar un cliente de la cola a un cocinero. */

    printf("\nCliente Atendido\n");

}

void deliverOrder(){
    /*Checkear si el juego esta iniciado.*/
    /*Checkear si hay ordenes completadas.*/

    printf("\nOrden Entregada\n");

}

void closeFoodPlace(){
    /*Cerrar todos los procesos pa irse a las casa a jugar wowcito y matar a mente colmena.*/
    printf("\nCerrando Local\n");
    placeOpen = 0;
    finished = 1;
}

void *streetThread(void *arg){
    
    FoodPlace *mercadoChino = (FoodPlace *)arg;
    placeOpen = 1;
    int tolerance = getMaxWaitTime(mercadoChino->menu);

    int aux = 0;

    while(placeOpen){
        pthread_t *cThread = (pthread_t *) calloc(1,sizeof(pthread_t));

        Client *client = (Client*)calloc(1,sizeof(Client));
        client->id = aux++;
        client->tolerance = tolerance;
        client->order = pickFood(mercadoChino->menu);
        client->mtx = mercadoChino->mtx;
        client->semQueue = mercadoChino->semQueue;

        sleep(rand()%10+1);
        pthread_create(cThread,NULL,clientThread,(void*)client);
    }

    printf("Cerrando hilo calle");

    pthread_exit(NULL);
}

void closeDoor(int sigCode){
    placeOpen = 0;
}

void *clientThread(void *arg){
    Client *client = (Client *)arg;

    printf("Nuevo cliente en cola");
    fflush(NULL);

    struct timespec wait;
    clock_gettime(CLOCK_REALTIME, &wait);
    wait.tv_sec += client->tolerance;

    int errCode = sem_timedwait(client->semQueue, &wait);

    if(!errCode){
        //IngresarPedido(client->m->lista_pedidos, client->order);
        pthread_mutex_lock(client->mtx);
        //SacarComida(cliente->m->lista_terminados, *client->order);
        //Pagar con memoria compartida sincronizado con split semaphore
    } else{
        printf("El Cliente %d se canso de esperar\r\n",client->id);
    }

    free(client);
    pthread_exit(NULL);
}

void *chefThread(void *arg){
    //int error;
    Chef *chef = (Chef *)arg;
    //Food comida;

    while(!finished){
        //semaforo
        //error= SacarPedido(chef->m,&comida);
        chef->libre = 0;
        //sleep(comida.prepTime);
        //error = IngresarComida(chef->m,*comida);
        pthread_mutex_unlock(chef->mtx);
        *chef->libre = 1;
    }
    // free(chef);
    pthread_exit(NULL);
}

Food *menuSetup(){
    Food *menu = calloc(10, sizeof(Food));

    sprintf(menu[0].name,"Pizza");
    menu[0].prepTime = 2;

    sprintf(menu[1].name,"Lomito");
    menu[1].prepTime = 2;

    sprintf(menu[2].name,"Empanadas");
    menu[2].prepTime = 5;

    sprintf(menu[3].name,"Ensalada");
    menu[3].prepTime = 4;

    sprintf(menu[4].name,"Milanesa");
    menu[4].prepTime = 3;

    sprintf(menu[5].name,"Sushi");
    menu[5].prepTime = 6;

    sprintf(menu[6].name,"Chop Suey");
    menu[6].prepTime = 3;

    sprintf(menu[7].name,"Pollo");
    menu[7].prepTime = 4;

    sprintf(menu[8].name,"Matambre");
    menu[8].prepTime = 3;

    sprintf(menu[9].name,"Choripan");
    menu[9].prepTime = 2;

    return menu;
}

Food *pickFood(Food *menu){
    return &menu[rand()%10];
}

int getMaxWaitTime(Food *menu){
    int max = menu[0].prepTime;

    for(int i = 0; i < 10; i++)
        if(menu[i].prepTime > max)
            max = menu[i].prepTime;
    return max * 4;
}