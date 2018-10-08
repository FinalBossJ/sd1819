#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "table-private.h"

/* Função de hash
* Devolve um int entre 0 e L, ou -1 em caso de erro
*/
int key_hash(char* key, int l){
    if (key == NULL || l < 0)
        return -1;

	int i, soma;
	for (i=0, soma=0; i < strlen(key); i++)
		soma += key[i];

    return soma % l;
}


/* Função para criar/inicializar uma nova tabela hash, com n  
 * linhas(n = módulo da função hash)
 */
struct table_t *table_create(int n) {
    if (n < 1)
        return NULL;

    struct table_t* table = (struct table_t*)malloc(sizeof(struct table_t));

    if (table == NULL)
        return NULL;

    table->head = (struct list_t**)malloc(n * (sizeof(struct list_t*)));

    if (table->head == NULL){
        free(table);
        return NULL;
    }

    int i = 0;
    for (i = 0; i < n; i++){
        table->head[i] = list_create();
        if (table->head[i] == NULL){ //libertar toda a previamente alocada
            int j = 0;
            for (j = 0; j < i; j++)
                free (table->head[j]);
            free(table->head);
            free(table);
            return NULL;
        }
            
    }

    table->size = n;
    return table;
}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser libertados
 * no contexto da função que chamou table_get.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key){
    if ( table == NULL || key == NULL)
        return NULL;
    
    int hash = key_hash(key, table->size);

    if (hash == -1)
        return NULL;
    
    struct entry_t* entry = list_get(table->head[hash], key);

    if (entry == NULL)
        return NULL;
    
    return data_dup(entry->value);
}


/* Libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t* table) {
    if (table != NULL){
        int i = 0;
        for (i = 0; i < table->size; i++){
            list_destroy(table->head[i]);
        }
        free(table->head);
        free(table);  
    }
}


/* Função para adicionar um par chave-valor na tabela.
 * Devolve 0 (ok) ou -1 (out of memory, outros erros)
 */
int table_put(struct table_t *table, char *key, struct data_t* value) {
    if (table == NULL || key == NULL || value == NULL)
        return -1;

    struct data_t* aux;

    if ((aux = table_get(table, key)) != NULL){
        data_destroy(aux);
        return -1;
    }

    struct entry_t* entry = entry_create(key, value);

    if (entry == NULL)
        return -1;
    
    int hash = key_hash(key, table->size);
    int result= list_add(table->head[hash], entry);

    entry_destroy(entry);
    return result;
}



/* Função para remover um par chave valor da tabela, especificado 
 * pela chave key, libertando a memória associada a esse par.
 * Devolve: 0 (OK), -1 (erro)
 */
int table_del(struct table_t* table, char* key){
    if ( table == NULL || key == NULL)
        return -1;

    int hash = key_hash(key, table->size);

    if (hash == -1)
        return -1;

    return list_remove(table->head[hash], key);
}


/* Devolve o número de pares chave/valor na tabela.
 */
int table_size(struct table_t* table) {
    if (table == NULL)
        return -1;
    
    int i = 0, soma = 0;

    for (i = 0; i < table->size; i++)
        soma += list_size(table->head[i]);

    return soma;
}


/* Devolve um array de char * com a cópia de todas as keys da tabela,
 * e um último elemento a NULL.
 */
char** table_get_keys(struct table_t* table) {
    if (table == NULL)
        return NULL;
    
    int i = 0, offset = 0, size = table_size(table);

    char** result = (char**) malloc( sizeof(char*) * (size + 1) );

    for (i = 0; i < table->size; i++){
        char** temp = list_get_keys(table->head[i]);
        int n = list_size(table->head[i]);
        memcpy(&result[offset], temp, sizeof(char*) * n );
        free(temp);
        offset += n;
    }

    result[size] = NULL;
    return result;
}

/* Liberta a memória alocada por table_get_keys().
 */
void table_free_keys(char** keys) {
	if ( keys != NULL)
        list_free_keys(keys);
}