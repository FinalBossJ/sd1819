#include <stdlib.h>
#include <string.h>

#include "list-private.h"

/* Cria uma nova lista. Em caso de erro, retorna NULL.
 */
struct list_t *list_create(){
	struct list_t* result = (struct list_t*)malloc( sizeof(struct list_t) );

	if (result == NULL)
		return NULL;
	
	result->size = 0;
	result->first = NULL;
	return result;
}


/* Elimina uma lista, libertando *toda* a memoria utilizada pela
 * lista.
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


/* Adiciona uma entry na lista. Como a lista deve ser ordenada, 
 * a nova entry deve ser colocada no local correto.
 * Retorna 0 (OK) ou -1 (erro)
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
		if ( strcmp(current->value->key, entry->key) < 0 ){
			if (before == NULL){
				list->first = node_create( entry, current );
			}else{
				before->next = node_create( entry, current );
			}
			list->size++;
			return 0;
		}else if ( strcmp(current->value->key, entry->key) == 0 ){
			data_destroy(current->value->value);
			current->value->value = data_dup(entry->value);
			return 0;
		}
		before = current;
		current = current->next;
	}

	//é o ultimo node
	before->next = node_create( entry, NULL);
	list->size++;
	return 0;
}


/* Elimina da lista um elemento com a chave key. 
 * Retorna 0 (OK) ou -1 (erro)
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


/* Obtem um elemento da lista que corresponda à chave key. 
 * Retorna a referência do elemento na lista (ou seja, uma alteração
 * implica alterar o elemento na lista). 
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


/* Retorna o tamanho (numero de elementos) da lista 
 * Retorna -1 em caso de erro.  */
int list_size(struct list_t* list){
	if (list == NULL)
		return -1;
	return list->size;
}


/* Devolve um array de char * com a cópia de todas as keys da 
 * tabela, e um último elemento a NULL.
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


/* Liberta a memoria reservada por list_get_keys.
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
struct node_t* node_create(struct entry_t* value, struct node_t* next){
	if (value == NULL)
		return NULL;
	
	struct node_t* result = (struct node_t*)malloc(sizeof(struct node_t));

	if (result == NULL)
		return NULL;

	result->value = entry_dup(value);
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