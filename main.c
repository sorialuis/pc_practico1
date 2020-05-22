#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "monitorBufferCircular.h"

#define COCINEROS 3
#define CLIENTES 50

/* Variables globales para la alarma */
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
void serveClient(FoodPlace *);
void deliverOrder(FoodPlace *);
void closeFoodPlace(Monitor_t *m);

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
    int finished_orders;

    pthread_t *chefThreads = (pthread_t *)calloc(COCINEROS,sizeof(pthread_t));

    srand(time(NULL));

    cooks = 3;
    // queued_clients = 5;
    placeOpen = 1;
    finished = 0;
    finished_orders = 3;

    MenuParameters *params = (MenuParameters*)calloc(1,sizeof(MenuParameters));
    
    params->mercadoChino = (FoodPlace *)calloc(1,sizeof(FoodPlace));
    params->mercadoChino->menu = menuSetup();
    params->cooks = &cooks;
    // params->queued_clients = &queued_clients; 
    params->mercadoChino->chefs = (Chef*)calloc(COCINEROS,sizeof(Chef));  
    params->mercadoChino->clients = (Client*)calloc(CLIENTES,sizeof(Client));

    params->mercadoChino->mtx = (pthread_mutex_t *) calloc(1,sizeof(pthread_mutex_t));
    params->mercadoChino->split = (SplitSemaphore_t *)calloc(1,sizeof(SplitSemaphore_t));
    params->mercadoChino->split->pagar = sem_open("/splitpagar", O_CREAT, O_RDWR, 0);
    if(params->mercadoChino->split->pagar == SEM_FAILED) {
        perror("/splitpagar");
        error = -1;
        return error;
    }

    params->mercadoChino->split->cobrar = sem_open("/splitcobrar", O_CREAT, O_RDWR, 0);
    if(params->mercadoChino->split->cobrar == SEM_FAILED) {
        perror("/splitcobrar");
        error = -1;
        return error;
    }

    params->mercadoChino->m = CrearMonitor();

    params->finished_orders = &finished_orders;
    pthread_mutex_init(params->mercadoChino->mtx,NULL);
    pthread_mutex_lock(params->mercadoChino->mtx);

    params->mercadoChino->semQueue = sem_open("/semQueue", O_CREAT, O_RDWR, 3);

    if(params->mercadoChino->semQueue == SEM_FAILED) {
        perror("/semQueue");
        error = -1;
        return error;
    }

    //memoria compartida 
    int memoria=0;
    memoria = shm_open("/memCompartida", O_CREAT | O_RDWR, 0660);
    if (memoria < 0) {
      perror("shm_open()");
      error = -1;
    }
    if (!error) {
        printf("Descriptor de memoria creado!\n");
        error = ftruncate(memoria, sizeof(int));
        if (error)
        perror("ftruncate()");
    }
    params->mercadoChino->memoria = (int *) calloc(1,sizeof(int));
    params->mercadoChino->memoria = &memoria;
    //Dentro del hilo solo le paso memoria 
    // int *datos=NULL;
    // datos = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);




    for(int i = 0; i < COCINEROS; i++){
        params->mercadoChino->chefs[i].id = i;
        params->mercadoChino->chefs[i].libre = (int *)calloc(1,sizeof(int));
        *params->mercadoChino->chefs[i].libre = 1;
        params->mercadoChino->chefs[i].m = params->mercadoChino->m;
        params->mercadoChino->chefs[i].mtx = params->mercadoChino->mtx;

        pthread_create(&chefThreads[i],NULL,chefThread,&params->mercadoChino->chefs[i]); 
    }

    pthread_create(&menu,NULL,showMenu,(void *)params);

        
    int opt;

    while (!finished){

        scanf("%d",&opt);
        /*El switch es feo... lo cambiamos ?*/
        switch (opt) {
            case 1:
                startGame(params->mercadoChino,params->sThread);
                break;
            case 2:
                serveClient(params->mercadoChino);
                break;
            case 3:
                deliverOrder(params->mercadoChino);
                break;
            case 4:
                closeFoodPlace(params->mercadoChino->m);
                break;
            default:
                printf("OPCION INCORRECTA");
        }
        fflush(stdin);
    }

    pthread_join(menu,NULL);

    return 1;
}

void *showMenu(void *args){
    MenuParameters *params;
    int i;
    char *status;

    int *datos=NULL;
    datos = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,*params->mercadoChino->memoria, 0);


    params = (MenuParameters *)args;

    while (!finished){
        clearScreen();
        printf("Puerta: %s\n\n", placeOpen ? "ABIERTA" : "CERRADA");

        printf("Estados de Cocineros:\n");
        for(i = 0; i < COCINEROS; i++ ){
            if(*params->mercadoChino->chefs[i].libre){
                status = "Desocupado";
            }else status = "Ocupado";
            printf("\tCocinero %d: %s\n",i+1,status);
        }

        printf("\nClientes en cola:\n");
        for(i = 0; i < params->mercadoChino->clientsTotal; i++ ){
            if(*params->mercadoChino->clients[i].tolerance){
                printf("\tCliente %d pedido %s.\n",params->mercadoChino->clients[i].id, params->mercadoChino->clients[i].order->name);
            }
        }
        printf("\nPedidos Terminados: %d\n",
            *params->mercadoChino->m->lista_terminados->cant);

        printf("\nGanancias: $%d\n",*datos);
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
    alarm(60);

}

void serveClient(FoodPlace *mercadoChino){

    /*Checkear si el juego esta iniciado.*/
    /*Checkear si hay clientes en la cola.*/
    /*Checkear si hay cocineros disponibles. */
    
    for(int i = 0; i < mercadoChino->clientsTotal; i++ ){
        if(*mercadoChino->clients[i].tolerance){
            sem_post(mercadoChino->semQueue);
            i = mercadoChino->clientsTotal;    
        }
    }

    

    /*Asignar un cliente de la cola a un cocinero. */
    return ;
}

void deliverOrder(FoodPlace *mercadoChino){
    /*Checkear si el juego esta iniciado.*/ 
    /*Checkear si hay ordenes completadas.*/ 
    if(*mercadoChino->m->lista_terminados->cant){
        sem_post(mercadoChino->split->pagar); 
        printf("Cobrando....\n");
        sem_wait(mercadoChino->split->cobrar);
    }
    return ;

}

void closeFoodPlace(Monitor_t *m){
    printf("\nCerrando Local\n");
    placeOpen = 0;
    finished = 1;
    BorrarMonitor(m);
}

void *streetThread(void *arg){
    
    FoodPlace *mercadoChino = (FoodPlace *)arg;
    placeOpen = 1;
    int tolerance = getMaxWaitTime(mercadoChino->menu);

    int aux = 0;

    while(placeOpen && aux < CLIENTES){
        pthread_t *cThread = (pthread_t *) calloc(1,sizeof(pthread_t));

        Client *client = (Client*)calloc(1,sizeof(Client));
        client->id = aux++;
        client->tolerance = (int*)calloc(1,sizeof(int));
        *client->tolerance = tolerance;
        client->order = pickFood(mercadoChino->menu);
        client->mtx = mercadoChino->mtx;
        client->semQueue = mercadoChino->semQueue;
        client->m = mercadoChino->m;
        client->memoria = mercadoChino->memoria;

        mercadoChino->clients[client->id] = *client;

        sleep(rand()%5+1);
        pthread_create(cThread,NULL,clientThread,(void*)client);
        mercadoChino->clientsTotal = aux;
    }

    printf("Cerrando hilo calle");

    pthread_exit(NULL);
}

void closeDoor(int sigCode){
    placeOpen = 0;
}

void *clientThread(void *arg){
    Client *client = (Client *)arg;

    int *datos=NULL;
    datos = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, *client->memoria, 0);

    printf("Nuevo cliente en cola");
    fflush(NULL);

    struct timespec wait;
    clock_gettime(CLOCK_REALTIME, &wait);
    wait.tv_sec += *client->tolerance;

    int errCode = sem_timedwait(client->semQueue, &wait);

    if(!errCode){
        *client->tolerance = 0;
        IngresarPedido(client->m, *client->order);
        pthread_mutex_lock(client->mtx);
        SacarComida(client->m, client->order);
        sem_wait(client->split->pagar); 
        //Guardando en memoria compartida
        *datos = *datos + client->order->value;
        sem_post(client->split->cobrar); 
        *client->m->lista_terminados->cant = *client->m->lista_terminados->cant - 1;
    } else{
        printf("El Cliente %d se canso de esperar\r\n",client->id);
        *client->tolerance = 0;
    }

    free(client);
    pthread_exit(NULL);

}

void *chefThread(void *arg){
    Chef *chef = (Chef *)arg;
    Food comida;
 
    while(!finished){
        //semaforo
        SacarPedido(chef->m,&comida);
        *chef->libre = 0;
        sleep(comida.prepTime);
        IngresarComida(chef->m,comida);
        *chef->m->lista_terminados->cant = *chef->m->lista_terminados->cant + 1;
        pthread_mutex_unlock(chef->mtx);
        *chef->libre = 1;
    }
    free(chef);
    pthread_exit(NULL);
    
}

Food *menuSetup(){
    Food *menu = calloc(10, sizeof(Food));

    sprintf(menu[0].name,"Pizza");
    menu[0].prepTime = 2;
    menu[0].value = 100;

    sprintf(menu[1].name,"Lomito");
    menu[1].prepTime = 2;
    menu[1].value = 300;


    sprintf(menu[2].name,"Empanadas");
    menu[2].prepTime = 5;
    menu[2].value = 150;

    sprintf(menu[3].name,"Ensalada");
    menu[3].prepTime = 4;
    menu[3].value = 100;

    sprintf(menu[4].name,"Milanesa");
    menu[4].prepTime = 3;
    menu[4].value = 150;

    sprintf(menu[5].name,"Sushi");
    menu[5].prepTime = 6;
    menu[5].value = 200;

    sprintf(menu[6].name,"Chop Suey");
    menu[6].prepTime = 3;
    menu[6].value = 200;

    sprintf(menu[7].name,"Pollo");
    menu[7].prepTime = 4;
    menu[7].value = 150;

    sprintf(menu[8].name,"Matambre");
    menu[8].prepTime = 3;
    menu[8].value = 350;

    sprintf(menu[9].name,"Choripan");
    menu[9].prepTime = 2;
    menu[9].value = 100;

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