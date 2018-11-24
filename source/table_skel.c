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

#include "message-private.h"
#include "table-private.h"
#include "persistent_table-private.h"

struct ptable_t* ptable;

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists, char *filename, int logsize){
	if(n_lists <= 0)
		return -1;
	struct table_t* table = table_create(n_lists);
	if(table == NULL)
		return -1;

	struct pmanager_t* pmanager = pmanager_create(filename, logsize);
	if(pmanager == NULL)
		return -1;


	ptable = ptable_create(table, pmanager);

	if(pmanager_have_data(pmanager) == 1){
		printf("Encontrados dados, tentando reconstruir tabela...\n");
		if(pmanager_fill_state(pmanager, table) == -1){
			printf("Erro ao reconstruir a tabela.\n");
			return -1;
		}
		printf("Tabela reconstruida com sucesso!\n");
	}else{
		printf("Nao foram encontrados dados, criando uma tabela nova com %d listas.\n", n_lists);
	}
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy(){
	ptable_destroy(ptable);
}

/* Executa uma operação na tabela (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada)
*/
int invoke(struct message_t *msg){
	struct message_t *msg_resposta;
	int result = 0, done = 0;
	/* Verificar parâmetros de entrada */
	if (msg == NULL || ptable == NULL){
		return -1;
	}

	/* Alocar espaço para a mensagem de resposta */
	msg_resposta = msg;
	if(msg_resposta == NULL) 
		return -1;

	
  	msg_resposta->opcode = msg->opcode + 1;
	/* Verificar opcode e c_type na mensagem de pedido e aplicar operação na tabela */
  	switch(msg->opcode) {
	    case OP_PUT+1:{
	      if(msg->c_type != CT_ENTRY){ 
	        msg_resposta->opcode = OP_ERROR;
	        msg_resposta->c_type = CT_NONE;
	      }else{
	        char* key = strdup(msg->content.entry->key);
	        struct data_t* value = data_dup(msg->content.entry->value);
	        result = ptable_put(ptable, key, value);
	        msg_resposta->c_type = CT_NONE;
	        done = 1;
	      }
	    }break;
	    case OP_GET+1:{
	      if(msg->c_type != CT_KEY){ 
	        msg_resposta->opcode = OP_ERROR;
	        msg_resposta->c_type = CT_NONE;
	      }else{
	        msg_resposta->c_type = CT_VALUE;
	        msg_resposta->content.value = ptable_get(ptable, msg->content.key);
	        /*Note que o caso em que uma chave não é encontrada no get não deve ser considerado
				como erro. Neste caso o servidor deve responder com uma resposta normal
				(OP_GET+1) mas definindo um data_t com size=0 e data=NULL.
	        */

	        if(msg_resposta->content.value != NULL)
	        	done = 1;
	      }
	    }break;
	    case OP_DEL+1:{
	      if(msg->c_type != CT_KEY){ 
	        msg_resposta->opcode = OP_ERROR;
	        msg_resposta->c_type = CT_NONE;
	      }else{
	        msg_resposta->c_type = CT_NONE;
	        result = ptable_del(ptable, msg->content.key);
	     	done = 1;
	    }
	    }break;
	    case OP_SIZE+1:{
	    	if(msg->c_type != CT_NONE){ 
		        msg_resposta->opcode = OP_ERROR;
		        msg_resposta->c_type = CT_NONE;
	      }else{
		      result = ptable_size(ptable);
		      msg_resposta->c_type = CT_RESULT;
		      msg_resposta->content.result = result;
		      done = 1;
	    }break;
	    case OP_GETKEYS+1:{
	     if(msg->c_type != CT_NONE){ 
	        msg_resposta->opcode = OP_ERROR;
	        msg_resposta->c_type = CT_NONE;
	      }else{
	        msg_resposta->c_type = CT_KEYS;
	        msg_resposta->content.keys = ptable_get_keys(ptable);
	        done = 1;
	      }
	    }break;
	  }
	}
	if(done == 0){
		msg_resposta->opcode = OP_ERROR;
		msg_resposta->c_type = CT_NONE;
	}else if (result < 0) {
		msg_resposta->opcode = OP_ERROR;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = -1;
	}
	return 0;
}