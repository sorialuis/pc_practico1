#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "MonitoresSimple.h"

struct Monitor_t* CrearMonitor  () {
  int error=0;
  struct Monitor_t *aux=NULL;

  struct boundedBuffer_t boundedBuffer;

  aux = (struct Monitor_t*)(calloc(1, sizeof(struct Monitor_t)));
  

  if (aux != NULL) {
    aux->ready=0;

    boundedBuffer.inicio = 0;
    boundedBuffer.fin    = 0;

    error += pthread_cond_init(&aux->cond, NULL);
    if (error)
      perror("pthread_cond_init()");

    error += CrearSemaforos(&aux->buffer);
    if (error)
      perror("CrearSemaforos()");

    pthread_cond_broadcast(&aux->cond);
    // pthread_mutex_unlock(&aux->mutex);
  }

  return aux;
}

int Ingresar(struct boundedBuffer_t *bb, Food dato){
    int error = 0;
    error = sem_wait(bb->vacio);
    if (!error) {
        error = sem_wait(bb->escribiendo);
    }
    if (!error) {      
        bb->buf[bb->fin] = dato;
        bb->fin = ++bb->fin % ELEMENTOS;
        error = sem_post(bb->escribiendo);
    }
    if (!error) {
        error = sem_post(bb->lleno);
        // usleep(rand() % 2000000);
    }
    if (error) {
        perror("HIJO sem_?");
        return -1;
    }
    return 0;
}

int Sacar(struct boundedBuffer_t *bb, Food *dato){
    int error=0, terminar=COCINEROS;
    Food leidos = NULL;

    error = sem_wait(bb->lleno);
    if (!error) {
        error = sem_wait(bb->leyendo);
    }
    if (!error){
        dato = bb->buf[bb->inicio];
        bb->inicio = ++bb->inicio % ELEMENTOS;
        error = sem_post(bb->leyendo);
    }else {
        perror("sem_wait()");
        return -1;
    }

    if (!error) {
        error = sem_post(bb->vacio);
    }
    if (error) {
        perror("sem_post()");
        return -1;
    }
    return 0;
}

int IngresarPedido (struct Monitor_t *m, Food dato) {
    return Ingresar(m->lista_pedidos,dato);
}

int SacarPedido (struct Monitor_t *m, Food *dato) {
    return Sacar(m->lista_pedidos,dato);
}

int IngresarComida(struct Monitor_t *m, Food dato){
    return Sacar(m->lista_terminados,dato);
}

int SacarComida(struct Monitor_t *m, Food *dato){
    return Sacar(m->lista_terminados,dato);
}

void BorrarMonitor (struct Monitor_t *m) {
  if (m != NULL) {
    pthread_cond_destroy(&m->cond);
    pthread_mutex_destroy(&m->mutex);
    free(m);
  }
}

int CrearSemaforos (struct boundedBuffer_t *bb) {
  int error=0;

  bb->ocupado = sem_open("/ocupado", O_CREAT, 0640, 0);
  if (bb->ocupado != SEM_FAILED) {
    printf("Semaforo [ocupado] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  bb->desocupado = sem_open("/desocupado", O_CREAT, 0640, COCINEROS);
  if (bb->ocupado != SEM_FAILED) {
    printf("Semaforo [desocupado] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  bb->pidiendo = sem_open("/pidiendo", O_CREAT, 0640, 1);
  if (bb->ocupado != SEM_FAILED) {
    printf("Semaforo [pidiendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  bb->cocinando = sem_open("/cocinando", O_CREAT, 0640, 1);
  if (bb->ocupado != SEM_FAILED) {
    printf("Semaforo [cocinando] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  return error;
}


