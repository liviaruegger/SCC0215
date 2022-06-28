#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct no no_t;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct no{
  elem info;
  no_t *proximo;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct lista{
  no_t *inicio;
  int tamanho;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
Lista_t *criar(){
  Lista_t *lista;

  lista = malloc(sizeof(Lista_t));

  if(lista != NULL){
    lista->inicio = NULL;
    lista->tamanho = 0;
  }

  return lista;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void desalocar(Lista_t *lista){
  struct no *n, *next;

  n = lista->inicio;

  do{
    if(n != NULL){
      next = n->proximo;
      free(n);
      n = next;
    }
  }while(n != NULL);

  free(lista);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void resetar(Lista_t *lista){
  struct no *n, *next;

  n = lista->inicio;

  do{
    if(n != NULL){
      next = n->proximo;
      free(n);
      n = next;
    }
  }while(n != NULL);

  lista->inicio = NULL;
  lista->tamanho = 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void inserir(Lista_t *lista, int size, long int offset){
  no_t *n = malloc(sizeof(no_t));

  n->info.size = size;
  n->info.offset = offset;
  n->proximo = NULL;

  no_t *aux = lista->inicio, *ant = NULL;

  while(aux != NULL && size <= aux->info.size)
  {
      ant = aux;
      aux = aux->proximo;
  }

  if(ant == NULL){
    n->proximo = lista->inicio;
    lista->inicio = n;
  }
  else{
    n->proximo = ant->proximo;
    ant->proximo = n;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void remover_ultimo(Lista_t *lista, elem *x){
  no_t *n;

  *x = lista->fim->info;

  free(lista->fim);
  lista->tamanho -= 1;

  n = lista->inicio;
  if(n != NULL){
    for(int i = 0; i < lista->tamanho - 1; i++){
      n = n->proximo;
    }
  }
  n->proximo = NULL;
  lista->fim = n;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void imprimir(Lista_t *lista){
  int inicio = 1;
  no_t *n;

  n = lista->inicio;
  printf("--- Lista ---\n");

  do{
    if(n != NULL){
      if(inicio != 1){
        printf(" ");
      }
      inicio = 0;

      printf("%ld / %d\n", n->info.offset, n->info.size);

      n = n->proximo;
    }
  } while(n != NULL);

  printf("\n");
}

void update_topo (Lista_t *lista, FILE *data_fp)
{
    int i = 0;
    no_t *n = lista->inicio;

    fseek(data_fp, 1, SEEK_SET); // vai para o campo 'topo'
    while (n != NULL) {
        fwrite(&n->info.offset, sizeof(long int), 1, data_fp);
        if (n->proximo != NULL)
        {
            fseek(data_fp, n->info.offset + 5, SEEK_SET);
        }
        n = n->proximo;
    }
}
