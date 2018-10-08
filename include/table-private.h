#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t{
	struct list_t** head; /* Primeira list da tabela */
	int size; /* Dimensão da tabela */
};

/* Função para criar/inicializar uma nova tabela hash, com n linhas 
 * (módulo da função HASH).
 */
struct table_t *table_create(int n);

/* Função de hash
* Devolve um int entre 0 e L, ou -1 em caso de erro
*/
int key_hash(char* key, int l);

void table_print(struct table_t *table);

#endif
