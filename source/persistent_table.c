/*
SD 2018/2019
Projecto 3 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "persistent_table-private.h"

/* Cria uma tabela persistente, passando como parâmetro a tabela
 * a ser mantida em memória e o gestor de persistência a ser usado para 
 * manter logs e checkpoints. 
 * Retorna a tabela persistente criada ou NULL em caso de erro.
 */
struct ptable_t *ptable_create(struct table_t *table, struct pmanager_t *pmanager){
	if(table == NULL || pmanager == NULL)
		return NULL;
	struct ptable_t* ptable = (struct ptable_t*)malloc(sizeof(struct ptable_t));
	if(ptable == NULL)
		return NULL;
	ptable->table = table;
	ptable->pmanager = pmanager;
	return ptable;
}

/* Liberta toda a memória ocupada pela tabela persistente e apaga
 * todos os ficheiros utilizados pela tabela.
 */
void ptable_destroy(struct ptable_t *ptable){
	table_destroy(ptable->table);
	pmanager_destroy(ptable->pmanager);
	free(ptable);
}

/* Função para adicionar um elemento na tabela.
 * A função vai *COPIAR* a key (string) e os dados para um novo espaço
 * de memória alocado por malloc().
 * Se a key já existir na tabela, a função tem de substituir a entrada
 * existente pela nova, fazendo a necessária gestão da memória para
 * armazenar os novos dados.
 * Devolve 0 (ok), em caso de adição ou substituição, ou -1 (erro), em
 * caso de erro.
 */
int ptable_put(struct ptable_t *ptable, char *key, struct data_t *data){
	if(ptable == NULL || key == NULL || data == NULL)
		return -1;

	
	if(table_put(ptable->table, key, data) == -1)
		return -1;

	struct message_t * msg = malloc(sizeof(struct message_t));
	msg->opcode = OP_PUT;
	msg->c_type = CT_ENTRY;
	if((msg->content.entry = (struct entry_t*) malloc(sizeof(struct entry_t))) == NULL) {
        free_message(msg);
        return -1;
	}
	msg->content.entry->key = strdup(key);
	msg->content.entry->value = data_dup(data);

	if(pmanager_log(ptable->pmanager, msg) == -1){
		if(pmanager_store_table(ptable->pmanager, ptable->table) == -1)
			return -1;

		if(pmanager_rotate_log(ptable->pmanager) == -1)
			return -1;
	}
	return 0;
}

/* Função para obter da tabela o valor associado à chave key.
 * A função aloca memória para armazenar uma *COPIA* dos dados da tabela
 * e retorna o endereço desta memória. O programa que chama esta função
 * é responsável por libertar esta memória.
 * Devolve NULL em caso de erro. 
 */
struct data_t *ptable_get(struct ptable_t *ptable, char *key){
	if(ptable == NULL || key == NULL)
		return NULL;

	return table_get(ptable->table, key);
}

/* Função para remover um elemento da tabela, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação ptable_put.
 * Retorna 0 (ok) ou -1 (key not found or error).
 */
int ptable_del(struct ptable_t *ptable, char *key){
	if(ptable == NULL || key == NULL)
		return -1;

	if(table_del(ptable->table, key) == -1)
			return -1;

	struct message_t * msg = malloc(sizeof(struct message_t));
	msg->opcode = OP_DEL;
	msg->c_type = CT_KEY;
	msg->content.key = strdup(key);

	if(pmanager_log(ptable->pmanager, msg) == -1){
		if(pmanager_store_table(ptable->pmanager, ptable->table) == -1)
			return -1;
		
		if(pmanager_rotate_log(ptable->pmanager) == -1)
			return -1;
	}

	return 0;
}

/* Devolve o número de elementos na tabela ou -1 em caso de erro.
 */
int ptable_size(struct ptable_t *ptable){
	if(ptable == NULL)
		return -1;

	return table_size(ptable->table);
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **ptable_get_keys(struct ptable_t *ptable){
	if(ptable == NULL)
		return NULL;

	return table_get_keys(ptable->table);
}

/* Função que liberta toda a memória alocada por ptable_get_keys().
 */
void ptable_free_keys(char **keys){
	table_free_keys(keys);
}