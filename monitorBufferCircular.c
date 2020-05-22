#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#include "monitorBufferCircular.h"


Monitor_t* CrearMonitor  () {
  int error=0;
  Monitor_t *aux=NULL;


  aux = (Monitor_t*)(calloc(1, sizeof(Monitor_t)));  
  aux->lista_pedidos = (boundedBuffer_t*)(calloc(1,sizeof(boundedBuffer_t)));
  aux->lista_terminados = (boundedBuffer_t*)(calloc(1,sizeof(boundedBuffer_t)));
  if (aux != NULL) {
  
    aux->lista_pedidos->inicio = 0;
    aux->lista_pedidos->fin    = 0;
    aux->lista_terminados->inicio = 0;
    aux->lista_terminados->fin    = 0;

    error += CrearSemaforos(aux);
    if (error)
      perror("CrearSemaforos()"); 
  }
  return aux;
}

int Ingresar(boundedBuffer_t *bb, Food dato){
    int error = 0;
    error = sem_wait(bb->vacio);
    if (!error) {
        error = sem_wait(bb->escribiendo);
    }
    if (!error) {      
        bb->buf[bb->fin] = dato;
        bb->fin = (bb->fin + 1) % ELEMENTOS;
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

int Sacar(boundedBuffer_t *bb, Food *dato){
    int error=0;

    error = sem_wait(bb->lleno);
    if (!error) {
        error = sem_wait(bb->leyendo);
    }
    if (!error){
        *dato = bb->buf[bb->inicio];
        bb->inicio = (bb->inicio + 1) % ELEMENTOS;
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

int IngresarPedido (Monitor_t *m, Food dato) {

    int error = Ingresar(m->lista_pedidos,dato);
    return error;


      
    
}

int SacarPedido (Monitor_t *m, Food *dato) {
    int error = Sacar(m->lista_pedidos,dato); 
    return error;
}

int IngresarComida(Monitor_t *m, Food dato){  
    int error = Ingresar(m->lista_terminados,dato);
    return error;
}

int SacarComida(Monitor_t *m, Food *dato){ 
    int error = Sacar(m->lista_terminados,dato);
    return error;
}

void BorrarMonitor (Monitor_t *m) {
  if (m != NULL) {
    int error=0, status=0;

    status = sem_close(m->lista_pedidos->lleno);
    if (!status) {
      status = sem_unlink("/listapedidos_lleno");
      if (!status)printf("Semaforo [listapedidos_lleno] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    status = sem_close(m->lista_pedidos->vacio);
    if (!status) {
      status = sem_unlink("/listapedidos_vacio");
      if (!status)printf("Semaforo [listapedidos_vacio] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    status = sem_close(m->lista_pedidos->leyendo);
    if (!status) {
      status = sem_unlink("/listapedidos_leyendo");
      if (!status)printf("Semaforo [listapedidos_leyendo] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    status = sem_close(m->lista_pedidos->escribiendo);
    if (!status) {
      status = sem_unlink("/listapedidos_escribiendo");
      if (!status)printf("Semaforo [listapedidos_escribiendo] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }


  status = sem_close(m->lista_terminados->lleno);
    if (!status) {
      status = sem_unlink("/listaterminados_lleno");
      if (!status)printf("Semaforo [listaterminados_lleno] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    status = sem_close(m->lista_terminados->vacio);
    if (!status) {
      status = sem_unlink("/listaterminados_vacio");
      if (!status)printf("Semaforo [listaterminados_vacio] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    status = sem_close(m->lista_terminados->leyendo);
    if (!status) {
      status = sem_unlink("/listaterminados_leyendo");
      if (!status)printf("Semaforo [listaterminados_leyendo] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    status = sem_close(m->lista_terminados->escribiendo);
    if (!status) {
      status = sem_unlink("/listaterminados_escribiendo");
      if (!status)printf("Semaforo [listaterminados_escribiendo] borrado!\n");
      else {
        perror("sem_unlink()");
        error -= 1;
      }
    }
    else {
      perror("sem_close()");
      error -= 1;
    }

    error = sem_unlink("/semQueue");
    if (error) {
      perror("/semQueue");
      error -= 1;
    }
    else {
      printf("/semQueue borrado!\n");
    }



    free(m);
  }
}

int CrearSemaforos (Monitor_t *m) {
  int error=0;

  m->lista_pedidos->cant = (int*)calloc(1,sizeof(int));
  *m->lista_pedidos->cant = 0;
  m->lista_terminados->cant = (int*)calloc(1,sizeof(int));
  *m->lista_terminados->cant = 0;
  m->lista_pedidos->lleno = sem_open("/listapedidos_lleno", O_CREAT, 0640, 0);
  if (m->lista_pedidos->lleno != SEM_FAILED) {
    printf("Semaforo [listapedidos_lleno] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_pedidos->vacio = sem_open("/listapedidos_vacio", O_CREAT, 0640, ELEMENTOS);
  if (m->lista_pedidos->vacio != SEM_FAILED) {
    printf("Semaforo [listapedidos_vacio] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_pedidos->leyendo = sem_open("listapedidos_leyendo", O_CREAT, 0640, 1);
  if (m->lista_pedidos->leyendo != SEM_FAILED) {
    printf("Semaforo [leyendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_pedidos->escribiendo = sem_open("/listapedidos_escribiendo", O_CREAT, 0640, 1);
  if ( m->lista_pedidos->escribiendo != SEM_FAILED) {
    printf("Semaforo [escribiendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_terminados->lleno = sem_open("/listaterminados_lleno", O_CREAT, 0640, 0);
  if (m->lista_terminados->lleno != SEM_FAILED) {
    printf("Semaforo [listapedidos_lleno] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_terminados->vacio = sem_open("/listaterminados_vacio", O_CREAT, 0640, ELEMENTOS);
  if (m->lista_terminados->vacio != SEM_FAILED) {
    printf("Semaforo [listapedidos_vacio] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_terminados->leyendo = sem_open("/listaterminados_leyendo", O_CREAT, 0640, 1);
  if (m->lista_terminados->leyendo != SEM_FAILED) {
    printf("Semaforo [leyendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }

  m->lista_terminados->escribiendo = sem_open("/listaterminados_escribiendo", O_CREAT, 0640, 1);
  if (m->lista_terminados->escribiendo != SEM_FAILED) {
    printf("Semaforo [escribiendo] creado!\n");
  }
  else {
    perror("sem_open()");
    error -= 1;
  }
  return error;
}



