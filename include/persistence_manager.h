#ifndef _PERSISTENCE_MANAGER_H
#define _PERSISTENCE_MANAGER_H

#include "table.h"
#include "message.h"

struct pmanager_t; /* A definir em persistence_manager-private.h */

/* Cria um gestor de persist�ncia que armazena logs em filename+".log" e o
 * estado do sistema em filename+".ckp". O par�metro logsize define o
 * tamanho m�ximo em bytes que o ficheiro de log pode ter e o par�metro
 * filename pode ser um path completo. 
 * Retorna o pmanager criado ou NULL em caso de erro.
 */
struct pmanager_t *pmanager_create(char *filename, int logsize);

/* Destr�i o gestor de persist�ncia passado como par�metro.
 * Esta fun��o n�o limpa os ficheiros de log e ckp do sistema.
 * Retorna 0 se tudo estiver OK ou -1 em caso de erro.
 */
int pmanager_destroy(struct pmanager_t *pmanager);

/* Faz o mesmo que a fun��o pmanager_destroy, mas apaga os ficheiros
 * de log e ckp geridos por este gestor de persist�ncia.
 * Retorna 0 se tudo estiver OK ou -1 em caso de erro.
 */
int pmanager_destroy_clear(struct pmanager_t *pmanager);

/* Verifica se os ficheiros de log e/ou ckp cont�m dados.
 * Retorna 1 caso existam dados nos ficheiros de log e/ou ckp e 0
 * caso contr�rio.
 */
int pmanager_have_data(struct pmanager_t *pmanager);

/* Escreve a opera��o op no fim do ficheiro de log associado a pmanager,
 * mas apenas se a dimens�o do ficheiro, ap�s a escrita, n�o ficar maior
 * que logsize. A escrita deve ser feita com o modo de escrita definido.
 * Retorna o n�mero de bytes escritos no log ou -1 no caso do ficheiro 
 * j� n�o ter espa�o suficiente para registar a opera��o.
 */
int pmanager_log(struct pmanager_t *pmanager, struct message_t *op);

/* Cria um ficheiro filename+".stt" com o estado de table. 
 * Retorna o tamanho do ficheiro criado ou -1 em caso de erro.*/
 
int pmanager_store_table(struct pmanager_t *pmanager, 
                         struct table_t *table);

/* Limpa o ficheiro ".log" e copia o ficheiro ".stt" para ".ckp". 
 * Retorna 0 se tudo correr bem ou -1 em caso de erro.*/
 
int pmanager_rotate_log(struct pmanager_t *pmanager);

/* Mete o estado contido nos ficheiros .log, .stt e/ou .ckp na tabela 
 * passada como argumento. Retorna 0 (ok) ou -1 em caso de erro.*/
 
int pmanager_fill_state(struct pmanager_t *pmanager, 
                        struct table_t *table);
#endif