#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *showMenu(void *);
void clearScreen();

void startGame();
void serveClient();
void deliverOrder();
void closeFoodPlace(int *open);

typedef struct {
    int *cooks; /*Cambiar por struct de cocineros*/
    int *queued_clients; /*Cambiar por struct de clientes con timeout y lo que pidieron*/
    int *finished_orders;
    int *open;
}MenuParameters;

int main() {
    pthread_t menu;

    int cooks;
    int queued_clients;
    int open;
    int finished_orders;
    MenuParameters *params;

    int opt;

    srand(time(NULL));

    cooks = 3;
    queued_clients = 5;
    open = 1;
    finished_orders = 3;

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