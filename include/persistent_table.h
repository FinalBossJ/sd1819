#ifndef _PERSISTENT_TABLE_H
#define _PERSISTENT_TABLE_H

#include "data.h"
#include "table.h"
#include "persistence_manager.h"

struct ptable_t; /* A definir em persistent_table-private.h */

/* Cria uma tabela persistente, passando como par�metro a tabela
 * a ser mantida em mem�ria e o gestor de persist�ncia a ser usado para 
 * manter logs e checkpoints. 
 * Retorna a tabela persistente criada ou NULL em caso de erro.
 */
struct ptable_t *ptable_create(struct table_t *table, 
                             struct pmanager_t *pmanager);

/* Liberta toda a mem�ria ocupada pela tabela persistente e apaga
 * todos os ficheiros utilizados pela tabela.
 */
void ptable_destroy(struct ptable_t *ptable);

/* Fun��o para adicionar um elemento na tabela.
 * A fun��o vai *COPIAR* a key (string) e os dados para um novo espa�o
 * de mem�ria alocado por malloc().
 * Se a key j� existir na tabela, a fun��o tem de substituir a entrada
 * existente pela nova, fazendo a necess�ria gest�o da mem�ria para
 * armazenar os novos dados.
 * Devolve 0 (ok), em caso de adi��o ou substitui��o, ou -1 (erro), em
 * caso de erro.
 */
int ptable_put(struct ptable_t *ptable, char *key, struct data_t *data);

/* Fun��o para obter da tabela o valor associado � chave key.
 * A fun��o aloca mem�ria para armazenar uma *COPIA* dos dados da tabela
 * e retorna o endere�o desta mem�ria. O programa que chama esta fun��o
 * � respons�vel por libertar esta mem�ria.
 * Devolve NULL em caso de erro. 
 */
struct data_t *ptable_get(struct ptable_t *ptable, char *key);

/* Fun��o para remover um elemento da tabela, indicado pela chave key,
 * libertando toda a mem�ria alocada na respetiva opera��o ptable_put.
 * Retorna 0 (ok) ou -1 (key not found or error).
 */
int ptable_del(struct ptable_t *ptable, char *key);

/* Devolve o n�mero de elementos na tabela ou -1 em caso de erro.
 */
int ptable_size(struct ptable_t *ptable);

/* Fun��o que devolve um array de char* com a c�pia de todas as keys da
 * tabela, colocando o �ltimo elemento do array com o valor NULL e
 * reservando toda a mem�ria necess�ria.
 */
char **ptable_get_keys(struct ptable_t *ptable);

/* Fun��o que liberta toda a mem�ria alocada por ptable_get_keys().
 */
void ptable_free_keys(char **keys);

#endif
