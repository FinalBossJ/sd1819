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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "message-private.h"
#include "table-private.h"
#include "persistence_manager-private.h"

/* Cria um gestor de persistência que armazena logs em filename+".log" e o
 * estado do sistema em filename+".ckp". O parâmetro logsize define o
 * tamanho máximo em bytes que o ficheiro de log pode ter e o parâmetro
 * filename pode ser um path completo. 
 * Retorna o pmanager criado ou NULL em caso de erro.
 */
struct pmanager_t *pmanager_create(char *filename, int logsize){
	if(filename == NULL || logsize <= 0)
		return NULL;

	struct pmanager_t* manager = (struct pmanager_t*)malloc(sizeof(struct pmanager_t));
	if(manager == NULL)
		return NULL;

	manager->logname = strcat(strdup(filename), ".log");
	manager->ckpname = strcat(strdup(filename), ".ckp");
	manager->sttname = strcat(strdup(filename), ".stt");
	manager->logsize = logsize;

	return manager;
}

/* Destrói o gestor de persistência passado como parâmetro.
 * Esta função não limpa os ficheiros de log e ckp do sistema.
 * Retorna 0 se tudo estiver OK ou -1 em caso de erro.
 */
int pmanager_destroy(struct pmanager_t *pmanager){
	if(pmanager == NULL)
		return -1;

	free(pmanager);
	return 0;
}

/* Faz o mesmo que a função pmanager_destroy, mas apaga os ficheiros
 * de log e ckp geridos por este gestor de persistência.
 * Retorna 0 se tudo estiver OK ou -1 em caso de erro.
 */
int pmanager_destroy_clear(struct pmanager_t *pmanager){
	if(pmanager == NULL)
		return -1;

	if(remove(pmanager->logname) == -1 || remove(pmanager->ckpname) == -1)
		return -1;

	free(pmanager);
	return 0;
}

/* Verifica se os ficheiros de log e/ou ckp contêm dados.
 * Retorna 1 caso existam dados nos ficheiros de log e/ou ckp e 0
 * caso contrário.
 */
int pmanager_have_data(struct pmanager_t *pmanager){
	if(pmanager == NULL)
		return -1;

	FILE *fp = fopen(pmanager->logname, "r");
	if(fp != NULL){	
		fseek(fp, 0, SEEK_END);

		if(ftell(fp) > 0){
			fclose(fp);
			return 1;
		}
		fclose(fp);
	}
	FILE *fp2 = fopen(pmanager->ckpname, "r");
	if(fp2 != NULL){	
		fseek(fp2, 0, SEEK_END);

		if(ftell(fp2) > 0){
			fclose(fp2);
			return 1;
		}
		fclose(fp2);
	}

	return 0;
}

/* Escreve a operação op no fim do ficheiro de log associado a pmanager,
 * mas apenas se a dimensão do ficheiro, após a escrita, não ficar maior
 * que logsize. A escrita deve ser feita com o modo de escrita definido.
 * Retorna o número de bytes escritos no log ou -1 no caso do ficheiro 
 * já não ter espaço suficiente para registar a operação.
 */
int pmanager_log(struct pmanager_t *pmanager, struct message_t *op){
	if(pmanager == NULL || op == NULL)
		return -1;

	int size;
	char message[1024];

	switch(op->opcode) {
	    case OP_PUT:{
	    	sprintf(message, "put %s %s\n", op->content.entry->key, (char *)op->content.entry->value->data);
	    }break;
	    case OP_DEL:{
	     	sprintf(message, "del %s\n", op->content.entry->key);
	    }break;
	  }
	
	struct stat st;
	stat(pmanager->logname, &st);
	size = st.st_size;

	if(size + sizeof(message) <= pmanager->logsize){
		FILE *fp = fopen(pmanager->logname, "a");
		
		if(fp == NULL)
			return -1;

		if(fprintf(fp, "%s" ,message) <= 0){
			fclose(fp);
			return -1;
		}
		fsync(fileno(fp));
		fclose(fp);
		return sizeof(message);
	}
	return -1;
}

/* Cria um ficheiro filename+".stt" com o estado de table. 
 * Retorna o tamanho do ficheiro criado ou -1 em caso de erro.
 */
int pmanager_store_table(struct pmanager_t *pmanager, struct table_t *table){
	if(pmanager == NULL || table == NULL)
		return -1;
	FILE *fp = fopen(pmanager->sttname, "w");
	int size, i;
	char** keys = table_get_keys(table);

	for(i = 0; keys[i] != NULL; i++) {
		if(fprintf(fp, "put %s %s\n", keys[i], (char *)table_get(table, keys[i])) <= 0){
			fclose(fp);
			return -1;
		}
	}
	fsync(fileno(fp));
	fclose(fp);
	struct stat st;
	stat(pmanager->sttname, &st);
	size = st.st_size;
	return size;
}



/* Limpa o ficheiro ".log" e copia o ficheiro ".stt" para ".ckp". 
 * Retorna 0 se tudo correr bem ou -1 em caso de erro.
 */
int pmanager_rotate_log(struct pmanager_t *pmanager){
	if(pmanager == NULL)
		return -1;

	if(remove(pmanager->logname) == -1)
		return -1;

	int i;
	FILE *fp1 = fopen(pmanager->sttname, "r");
    if(fp1 == NULL){
      return -1;
    }

    FILE *fp2 = fopen(pmanager->ckpname, "w");
    if (fp2 == NULL){
      fclose(fp1);
      return -1;
    }
    while((i = fgetc(fp1)) != EOF){
      fputc(i, fp2);
    }

    fclose(fp1);
    fclose(fp2);
    return 0;
}

/* Mete o estado contido nos ficheiros .log, .stt e/ou .ckp na tabela 
 * passada como argumento. Retorna 0 (ok) ou -1 em caso de erro.
 */
int pmanager_fill_state(struct pmanager_t *pmanager, struct table_t *table){
	if(pmanager == NULL || table == NULL)
		return -1;
	
	/* Int a determinar qual ficheiro ler
	* 0 = nenhum
	* 1 = stt
	* 2 = ckp
	* 3 = log
	*/
	int bkp = 0;

	FILE *fp = fopen(pmanager->sttname, "r");
	if(fp != NULL){	
		fseek(fp, 0, SEEK_END);

		if(ftell(fp) > 0){
			bkp = 1;
		}else{
			fclose(fp);
		}
	}
	if(bkp != 0){
		fp = fopen(pmanager->ckpname, "r");
		if(fp != NULL){	
			fseek(fp, 0, SEEK_END);

			if(ftell(fp) > 0){
				bkp = 2;
			}else{
				fclose(fp);
			}
		}
	}
	if(bkp != 0){
		fp = fopen(pmanager->logname, "r");
		if(fp != NULL){	
			fseek(fp, 0, SEEK_END);

			if(ftell(fp) > 0){
				bkp = 3;
			}else{
				fclose(fp);
			}
		}
	}

	if(bkp == 0)
		return 0;
	rewind(fp);


	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, fp) != -1) {
         char* command = strtok(line, " ");
         if(strncmp(command, "put", 3) == 0){
         	struct data_t *value;
            char *key = strtok(NULL, " ");
            char* data = strtok(NULL, "");
            if((value = data_create2(sizeof(data), data)) == NULL) {
                free(value);
                return -1;
            }

            if(table_put(table, key, value) == -1){
            	free(value);
            	return -1;
            }
         }else if(strncmp(command, "del", 3) == 0){
         	char *key = strtok(NULL, " ");
         	if(table_del(table, key) == -1){
         		return -1;
         	}
         }
    }

    return 0;
}