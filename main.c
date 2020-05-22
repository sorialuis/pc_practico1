#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/* Thread functions */
void *streetThread(void *);

/* UserMenu Functions */
void *showMenu(void *);
void clearScreen();

/* UserMenu Options */
void startGame();
void serveClient();
void deliverOrder();
void closeFoodPlace(int *open);

typedef struct {
    char name[50];
    int prepTime;
}Food;

typedef struct {
    pthread_mutex_t mtxCook;
    pthread_mutex_t mtxCheckout;

}Cashier;

typedef struct {
    int *open;
    Food *menu;
    Food *currentOrder;
    Cashier cashier;
    pthread_mutex_t mtxQueue;
    pthread_mutex_t mtxServed;
}FoodPlace;

typedef struct {
    int *cooks; /*Cambiar por struct de cocineros*/
    int *queued_clients; /*Cambiar por struct de clientes con timeout y lo que pidieron*/
    int *finished_orders;
    int *open;
}MenuParameters;

/* FoodMenu Functions */
Food *menuSetup();
Food *pickFood(Food * menu);
int getMaxWaitTime(Food *menu);


int main() {
    pthread_t menu;

    int cooks;
    int queued_clients;
    int open;
    int finished_orders;
    MenuParameters *params;
    int opt;

    FoodPlace *mercadoChino = (FoodPlace *)calloc(1,sizeof(FoodPlace));



    srand(time(NULL));

    cooks = 3;
    queued_clients = 5;
    open = 1;
    finished_orders = 3;

    mercadoChino->menu = menuSetup();
    mercadoChino->open = &open;

    params = calloc(1,sizeof(MenuParameters));
    params->cooks = &cooks;
    params->queued_clients = &queued_clients;
    params->open = &open;
    params->finished_orders = &finished_orders;

    pthread_create(&menu,NULL,showMenu,(void *)params);

    while (open){

        scanf("%d",&opt);
        /*El switch es feo... lo cambiamos ?*/

        switch (opt) {
            case 1:
                startGame();
                break;

            case 2:
                serveClient();
                break;

            case 3:
                deliverOrder();
                break;

            case 4:
                closeFoodPlace(&open);
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

    while (*params->open){
        clearScreen();

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

        fflush(stdin);
        sleep(2);
    }

    pthread_exit(NULL);
}

void clearScreen()
{
    system("@cls||clear");
}

void startGame(){
    /*Si el juego no esta iniciado, Ejecutar la calle*/

    printf("\nJuego Iniciado\n");
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

void closeFoodPlace(int *open){
    /*Cerrar todos los procesos pa irse a las casa a jugar wowcito y matar a mente colmena.*/
    printf("\nCerrando Local\n");
    *open = 0;
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
    int min = menu[0].prepTime;

    for(int i = 0; i < 10; i++)
        if(menu[i].prepTime < min)
            min = menu[i].prepTime;
    return min * 4;
}