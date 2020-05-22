#ifndef _MONITORSIMPLE_H_
#define _MONITORSIMPLE_H_

#include "structs.h"

struct Monitor_t* CrearMonitor  ();
int Ingresar(boundedBuffer_t *bb, Food dato);
int Sacar(boundedBuffer_t *bb, Food *dato);
int IngresarPedido (Monitor_t *m, Food dato);
int SacarPedido (Monitor_t *m, Food *dato);
int IngresarComida(Monitor_t *m, Food dato);
int SacarComida(Monitor_t *m, Food *dato);
void BorrarMonitor(Monitor_t *m);

#endif
