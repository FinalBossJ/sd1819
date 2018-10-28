/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "list.h"

struct node_t{
	struct entry_t* value;	/* entry do node */
	struct node_t* next; /* proximo elemento */
};

struct list_t{
	struct node_t* first; /* primeiro node da lista */
	int size; /*tamanho da lista */
};

/* Função que cria um novo node (isto é, que inicializa
 * a estrutura e aloca a memória necessária).
 */
struct node_t* node_create(struct entry_t* value, struct node_t* next);


/* Função que destrói um node (não destroi outros nodes ligados ao mesmo)
 */
void node_destroy(struct node_t* node);

#endif
