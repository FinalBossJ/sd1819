/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/

#include <stdlib.h>
#include <string.h>

#include "entry.h"

/* Função que cria uma entry, reservando a memória necessária para a
* estrutura e inicializando os campos key e value, respetivamente, com a
* string e o bloco de dados passados como parâmetros, sem reservar
* memória para estes campos.
 */
struct entry_t *entry_create(char *key, struct data_t *data){
	if (key == NULL || data == NULL )
		return NULL;
	
	struct entry_t* result = (struct entry_t*)malloc( sizeof(struct entry_t) );

	if (result == NULL)
		return NULL;


	result->key = key;

	result->value = data;
	

	return result;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
	if (entry != NULL){
		free(entry->key);
		data_destroy(entry->value);
		free(entry);
	}
}


/*  Função que duplica uma entry, reservando a memória necessária para a
* nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t* entry){
	if (entry == NULL)
		return NULL;
	
	char *key = strdup(entry->key);

	if (key == NULL){
		free(key);
		return NULL;
	}

	struct data_t *value = data_dup(entry->value);

	if (value == NULL){
		free(key);
		free(value);
		return NULL;
	}


	struct entry_t* result = entry_create(key, value);

	if (result == NULL)
		return NULL;

	return result;
}