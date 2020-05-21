#ifndef _MONITORSIMPLE_H_
#define _MONITORSIMPLE_H_

#define COCINEROS 3
#define ELEMENTOS 50

#include "structs.h"

struct Monitor_t{
    Food pedido;
    boundedBuffer_t lista_pedidos;
    boundedBuffer_t lista_terminados;
};

struct boundedBuffer_t{
  int inicio, fin;
  Food buf[ELEMENTOS];
  sem_t *ocupado, *desocupado, *pidiendo, *cocinando;
//     sem_t *lleno, *vacio, *leyendo, *escribiendo;
};

struct Monitor_t* CrearMonitor  ();
int LeerPedido(struct Monitor_t *m, Food dato);
int GuardarPedido(struct Monitor_t *m, Food *dato);
void BorrarMonitor(struct Monitor_t *m);

#endif
