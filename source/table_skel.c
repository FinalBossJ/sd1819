/*
SD 2018/2019
Projecto 2 - Grupo 32
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

struct table_t* table;

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists){
	if(n_lists <= 0)
		return -1;
	table = table_create(n_lists);
	if(table == NULL)
		return -1;
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy(){
	table_destroy(table);
}

/* Executa uma operação na tabela (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada)
*/
int invoke(struct message_t *msg){
	struct message_t *msg_resposta;
	int result = 0, done = 0;
	/* Verificar parâmetros de entrada */
	if (msg == NULL || table == NULL){
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
	        result = table_put(table, key, value);
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
	        msg_resposta->content.value = table_get(table, msg->content.key);
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
	        result = table_del(table, msg->content.key);
	     	done = 1;
	    }
	    }break;
	    case OP_SIZE+1:{
	    	if(msg->c_type != CT_NONE){ 
		        msg_resposta->opcode = OP_ERROR;
		        msg_resposta->c_type = CT_NONE;
	      }else{
		      result = table_size(table);
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
	        msg_resposta->content.keys = table_get_keys(table);
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