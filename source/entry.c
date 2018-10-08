
#include <stdlib.h>
#include <string.h>

#include "entry.h"

/* Função que cria um novo par {chave, valor} (isto é, que inicializa
 * a estrutura e aloca a memória necessária).
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

/* Função que destrói um par {chave-valor} e liberta toda a memória.
 */
void entry_destroy(struct entry_t *entry){
	if (entry != NULL){
		free(entry->key);
		data_destroy(entry->value);
		free(entry);
	}
}


/* Função que duplica um par {chave, valor}.
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