/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#include <stdlib.h>
#include <string.h>

#include "list-private.h"

/* Função que cria uma nova lista (estrutura list_t a ser definida pelo
* grupo no ficheiro list-private.h).
* Em caso de erro, retorna NULL.
 */
struct list_t *list_create(){
	struct list_t* result = (struct list_t*)malloc( sizeof(struct list_t) );

	if (result == NULL)
		return NULL;
	
	result->size = 0;
	result->first = NULL;
	return result;
}


/*  Função que elimina uma lista, libertando *toda* a memoria utilizada
* pela lista.
 */
void list_destroy(struct list_t* list){
	if ( list != NULL){
		struct node_t* current = list->first;
		struct node_t* aux;

		while (current != NULL){
			aux = current->next;
			node_destroy(current);
			current = aux;
		}
		free(list);
	}
}


/** Função que adiciona no final da lista (tail) a entry passada como
* argumento.
* Retorna 0 (OK) ou -1 (erro).
 */
int list_add(struct list_t *list, struct entry_t *entry){
	if (list == NULL || entry == NULL)
		return -1;

	struct node_t* current = list->first;
	struct node_t* before = NULL;

	if (current == NULL){
		//the list is empty	
		list->first = node_create( entry, NULL );
		list->size++;
		return 0;
	}

	while (current != NULL){
		//Caso exista na lista uma entry com key igual a que queremos inserir,
		//os dados da entry (value) já existente na lista serão substituídos pelos os da nova entry.
		if(strcmp(current->value->key, entry->key) == 0){
			before->next = node_create(entry, current->next);
			list->size++;
			node_destroy(current);
			return 0;
		}
		before = current;
		current = current->next;
		
	}

	//é o ultimo node
	before->next = node_create(entry, NULL);
	list->size++;
	return 0;
}


/* Função que elimina da lista a entry com a chave key.
* Retorna 0 (OK) ou -1 (erro).
 */
int list_remove(struct list_t* list, char* key){
	if (list == NULL || key == NULL)
		return -1;
	
	struct node_t* current = list->first;
	struct node_t* before = NULL;

	while (current != NULL){
		if ( strcmp(current->value->key, key) == 0 ){
			if (before == NULL){
				list->first = current->next;
			}else{
				before->next = current->next;
			}
			node_destroy(current);
			list->size--;
			return 0;
		}
		before = current;
		current = current->next;
	}
	return -1;
}


/*  Função que obtém da lista a entry com a chave key.
* Retorna a referência da entry na lista ou NULL em caso de erro.
* Obs: as funções list_remove e list_destroy vão libertar a memória
* ocupada pela entry ou lista, significando que é retornado NULL
* quando é pretendido o acesso a uma entry inexistente.
 */
struct entry_t *list_get(struct list_t* list, char* key){
	if ( list == NULL || key == NULL )
		return NULL;
	
	struct node_t* current = list->first;

	while (current != NULL){
		if ( strcmp(current->value->key, key) == 0 )
			return current->value;
		current = current->next;
	}
	return NULL;
}


/* Função que retorna o tamanho (número de elementos (entries)) da lista,
* ou -1 (erro).
*/
int list_size(struct list_t* list){
	if (list == NULL)
		return -1;
	return list->size;
}


/* Devolve um array de char * com a cópia de todas as keys da 
 * tabela, colocando o último elemento do array com o valor NULL e
* reservando toda a memória necessária.
 */
char** list_get_keys(struct list_t* list){
	if (list == NULL)
		return NULL;
	
	char** result = (char**)malloc( (list->size + 1) * sizeof(char*) );

	if (result == NULL)
		return NULL;

	struct node_t* node = list->first;
	int i = 0;

	while ( node != NULL ){
		result[i] = strdup( node->value->key );

		if (result[i] == NULL)
			return NULL;
		i++;
		node = node->next;
	}

	result[i] = NULL;
	return result;
}


/* Função que liberta a memória ocupada pelo array das keys da lista,
* obtido pela função list_get_keys.
*/
void list_free_keys(char** keys){
	if ( keys != NULL){
		int i = 0;
		while ( keys[i] != NULL ){
			free(keys[i]);
			i++;
		}
		free(keys);
	}
}

/* Função que cria um novo node (isto é, que inicializa
 * a estrutura e aloca a memória necessária).
 */
struct node_t *node_create(struct entry_t* value, struct node_t* next){
	if (value == NULL)
		return NULL;
	
	struct node_t* result = (struct node_t*)malloc(sizeof(struct node_t));

	if (result == NULL)
		return NULL;

	result->value = value;
	result->next = next;
	return result;
}


/* Função que destrói um node (não destroi outros nodes ligados ao mesmo)
 */
void node_destroy(struct node_t* node){
	if (node != NULL){
		entry_destroy(node->value);
		free(node);
	}
}