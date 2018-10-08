#include <stdlib.h>
#include <string.h>

#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size 
 */
struct data_t *data_create(int size){
	if (size <= 0)
		return NULL;
	
	struct data_t* result = (struct data_t*)malloc( sizeof ( struct data_t ) );
	
	if (result == NULL)
		return NULL;
	
	result->data = (void*)malloc(size);
	
	if (result->data == NULL){
		free(result);
		return NULL;
	}
	
	result->datasize = size;
	return result;
}


/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data){
	if ( data == NULL || size <= 0)
		return NULL;

	struct data_t* result = data_create(size);

	if (result == NULL)
		return NULL;

	result->data = data;
	return result;
}


/* Função que destrói um bloco de dados e liberta toda a memória.
 */
void data_destroy(struct data_t* data){
	if ( data != NULL ){
		if ( data->data != NULL)
			free (data->data);
		free(data);
	}
}


/* Função que duplica uma estrutura data_t.
 */
struct data_t *data_dup(struct data_t* data){
	if (data == NULL || data->data == NULL)
		return NULL;

	struct data_t* p;
    if((p = data_create(data->datasize)) == NULL)
        return NULL;

	memcpy(p->data, data->data, data->datasize);

	return p;
}