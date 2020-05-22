#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    int error=0;
    char str[50];

    error = sem_unlink("/listapedidos_lleno");
    if (error) {
      perror("/listapedidos_lleno");
    }
    else {
      printf("/listapedidos_lleno borrado!\n");
    }

    error = sem_unlink("/listapedidos_vacio");
    if (error) {
      perror("/listapedidos_vacio");
    }
    else {
      printf("/listapedidos_vacio borrado!\n");
    }

    error = sem_unlink("/listapedidos_leyendo");
    if (error) {
      perror("/listapedidos_leyendo");
    }
    else {
      printf("/listapedidos_leyendo borrado!\n");
    }

    error = sem_unlink("/listapedidos_escribiendo");
    if (error) {
      perror("/listapedidos_escribiendo");
    }
    else {
      printf("/listapedidos_escribiendo borrado!\n");
    }



    error = sem_unlink("/listaterminados_lleno");
    if (error) {
      perror("/listaterminados_lleno");
    }
    else {
      printf("/listaterminados_lleno borrado!\n");
    }

    error = sem_unlink("/listaterminados_vacio");
    if (error) {
      perror("/listaterminados_vacio");
    }
    else {
      printf("/listaterminados_vacio borrado!\n");
    }

    error = sem_unlink("/listaterminados_leyendo");
    if (error) {
      perror("/listaterminados_leyendo");
    }
    else {
      printf("/listaterminados_leyendo borrado!\n");
    }

    error = sem_unlink("/listaterminados_escribiendo");
    if (error) {
      perror("/listaterminados_escribiendo");
    }
    else {
      printf("/listaterminados_escribiendo borrado!\n");
    }
  return error;
}