/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "message.h"
#include "message-private.h"

#include "base64.c"

#include "table.h"  /* for table_free_keys() */

/* Liberta a memoria alocada na função buffer_to_message para a
 * struct message_t.
 */
void free_message(struct message_t *msg){
  if (msg != NULL){
    switch (msg->c_type){
      case CT_VALUE: data_destroy(msg->content.value); break;
      case CT_ENTRY:
				data_destroy(msg->content.entry->value);
				free(msg->content.entry->key);
				break;
	case CT_KEYS: table_free_keys(msg->content.keys); break;
	case CT_KEY: free(msg->content.key); break;
    }
    free(msg);
  }
}

/* Função que garante o envio de len bytes armazenados em buf,
   através da socket sock.
*/
int write_all(int sock, char *buf, int len){
    int bufsize = len;

    while(len>0) {
    
        int res = write(sock, buf, len);
        if(res<0) {
            if(errno==EINTR) continue;
            return res;
        }
        buf += res;
        len -= res;
    }
    return bufsize;
}


/* Função que garante a receção de len bytes através da socket sock,
   armazenando-os em buf.
*/
int read_all(int sock, char *buf, int len){
  int bufsize = len;
    while(len>0) {
        int res = read(sock, buf, len);
        if(res<=0) {
            if(errno==EINTR) continue;
            return res;
        }
        buf += res;
        len -= res;
    }
    return bufsize;
}

/* Serializa o conteúdo de mensagem passada em message_t, colocando a
 * mensagem serializada como uma sequência de bytes num buffer cujo espaço
 * deverá ser alocado e cujo endereço deverá ser colocado no char* apontado
 * por msg_buf. Retorna o tamanho do buffer alocado, ou -1 em caso de erro.
 * A mensagem serializada numa sequência de bytes tem o seguinte formato:
 *
 *  OPCODE      C_TYPE      CONTENT
 * [2 bytes]   [2 bytes]   [CT bytes]
 *  
 * em que OPCODE corresponde ao código da operação (opcode) a realizar na
 * tabela do servidor, C_TYPE corresponde ao tipo de conteúdo da mensagem
 * e CONTENT corresponde ao conteúdo da mensagem.
 *
 * CONTENT depende de C_TYPE, da seguinte forma: 
 *
 * CT_KEY:        KEYSIZE(KS)     KEY
 *                 [2 bytes]   [KS bytes]
 *
 * CT_VALUE:      DATASIZE(DS)    DATA
 *                 [4 bytes]   [DS bytes]
 *
 * CT_ENTRY:      KEYSIZE(KS)     KEY      DATASIZE(DS)     DATA
 *                 [2 bytes]   [KS bytes]   [4 bytes]    [DS bytes]
 *
 * CT_KEYS:         NKEYS    KEYSZ_1     KEY_1    ...  KEYSZ_N     KEY_N
 *                [4 bytes] [2 bytes] [KS1 bytes] ... [2 bytes] [KSN bytes]
 *
 * CT_RESULT:      RESULT
 *                [4 bytes]
 *
 * Notar que o `\0´ no fim das strings (chaves) e o NULL no fim do array de
 * chaves não são enviados nas mensagens.
 *
 * Notar também que o bloco de dados binários correspondentes a DATA deve
 * ser codificado no formato BASE64. Para tal recomenda-se o uso da
 * biblioteca base64.h e base64.c (disponível em 
 * http://josefsson.org/base-encoding/)
 */
int message_to_buffer(struct message_t *msg, char **msg_buf){
  	int buffer_size = _SHORT + _SHORT + _SHORT;
  	int i, k, int_v;
  	void *encodedData;
  	short short_v;
  	char *ptr;
  	if (msg==NULL)
    	return -1;

	switch (msg->c_type){
		case CT_VALUE:
			buffer_size += _INT;
			buffer_size += msg->content.value->datasize;
			break;
		case CT_ENTRY:
			buffer_size += _SHORT + _INT;
			buffer_size += strlen(msg->content.entry->key);
			buffer_size += msg->content.entry->value->datasize;
			break;
		case CT_KEYS:
			buffer_size += _INT;
			i = 0;
			while (msg->content.keys[i] != NULL)
				buffer_size += _SHORT + strlen(msg->content.keys[i++]);
			k = i;
			break;
		case CT_KEY:
			buffer_size += _SHORT + strlen(msg->content.key);
			break;
		case CT_RESULT:
			buffer_size += _INT;
			break;	
	}
	if ((*msg_buf = (char *) malloc(buffer_size)) == NULL)
	  return -1;
	ptr = *msg_buf;

	short_v = htons(msg->opcode);
	memcpy(ptr, &short_v, _SHORT);
	ptr += _SHORT;

	short_v = htons(msg->c_type);
	memcpy(ptr, &short_v, _SHORT);
	ptr += _SHORT;

	switch (msg->c_type){
		case CT_RESULT:
			int_v =  htonl(msg->content.result);
			memcpy(ptr, &int_v, _INT);
			break;
		case CT_KEY:
			short_v = htons(strlen(msg->content.key));
			memcpy(ptr, &short_v, _SHORT);
			ptr += _SHORT;
			memcpy(ptr, msg->content.key, strlen(msg->content.key));
			break;
		case CT_KEYS:
			int_v = htonl(k);
			memcpy(ptr, &int_v, _INT);
			ptr += _INT;
			i = 0;
			while (msg->content.keys[i] != NULL){
				short_v = htons(strlen(msg->content.keys[i]));
				memcpy(ptr, &short_v, _SHORT);
				ptr += _SHORT;
				memcpy(ptr, msg->content.keys[i], strlen(msg->content.keys[i]));
				ptr += strlen(msg->content.keys[i++]);
			}
			break;
		case CT_VALUE:
			int_v = htonl(msg->content.value->datasize);
			memcpy(ptr, &int_v, _INT);
			ptr += _INT;

			encodedData = malloc(msg->content.value->datasize);
			if(encodedData == NULL)
				return -1;

			base64_encode(msg->content.value->data, msg->content.value->datasize, encodedData, msg->content.value->datasize);
			memcpy(ptr, encodedData, msg->content.value->datasize);
			break;
		case CT_ENTRY:
			short_v = htons(strlen(msg->content.entry->key));
			memcpy(ptr, &short_v, _SHORT);
			ptr += _SHORT;
			memcpy(ptr, msg->content.entry->key, strlen(msg->content.entry->key));
			ptr += strlen(msg->content.entry->key);
			int_v = htonl(msg->content.entry->value->datasize);
			memcpy(ptr, &int_v, _INT);
			ptr += _INT;

			encodedData = malloc(msg->content.entry->value->datasize);
			if(encodedData == NULL)
				return -1;

			base64_encode(msg->content.entry->value->data, msg->content.entry->value->datasize, encodedData, msg->content.entry->value->datasize);
			memcpy(ptr, encodedData, msg->content.entry->value->datasize);
			break;
	}
  return buffer_size;
}


/* De-serializa a mensagem contida em msg_buf, com tamanho msg_size,
 * colocando-a e retornando-a numa struct message_t, cujo espaço em
 * memória deve ser reservado.
 */
struct message_t *buffer_to_message(char *msg_buf, int msg_size){
  struct message_t *m;
  struct data_t *d;
  char *key;
  int i, ks, k;
  short ss;

  if (msg_buf == NULL)
    return NULL;

  if ((m = (struct message_t *) malloc(sizeof(struct message_t))) == NULL)
    return NULL;

  m->opcode = ntohs(*(short *) msg_buf++);
  m->c_type = ntohs(*(short *) ++msg_buf);
  /*if ((m->opcode < 10) || (m->opcode > 50)){
    free_message(m);
    return NULL;
  }*/

  msg_buf += _SHORT;
	switch (m->c_type){
		case CT_RESULT:
			m->content.result = ntohl(*(int *) msg_buf);
			return m;
		case CT_KEY:
			ks = (int) ntohs(*(short *) msg_buf);
			msg_buf += _SHORT;
			if ((m->content.key = (char *) malloc((ks * sizeof(char)) + 1)) == NULL){
				free(m);
				return NULL;
			}
			memcpy(m->content.key, msg_buf, ks);
			m->content.key[ks] = '\0';
			return m;
		case CT_KEYS:
			ks = ntohl(*(int *) msg_buf);
			msg_buf += _INT;
			if ((m->content.keys = (char **) malloc((ks + 1) * sizeof(char *))) == NULL){
				free(m);
				return NULL;
			}
			for (i = 0; i < ks; i++){
				ss = ntohs(*(short *) msg_buf);
				msg_buf += _SHORT;
				if ((m->content.keys[i] = malloc((ss * sizeof(char)) + 1)) == NULL){
					for (k = 0; k < i; k++)
						free(m->content.keys[k]);
					free(m);
					return NULL;
				}
				memcpy(m->content.keys[i], msg_buf, ss);
				m->content.keys[i][ss] = '\0';
				msg_buf += ss;
			}
			m->content.keys[i] = NULL;
			return m;
		case CT_VALUE:
			ks = ntohl(*(int *) msg_buf);
			msg_buf += _INT;
			if ((m->content.value = data_create(ks)) == NULL){
				free(m);
				return NULL;
			}

			base64_decode(msg_buf, ks, m->content.value->data, &ks);
			return m;
		case CT_ENTRY:
			ss = ntohs(*(short *) msg_buf);
			msg_buf += _SHORT;
			if ((key = malloc((ss * sizeof(char)) + 1)) == NULL){
				free(m);
				return NULL;
			}
			memcpy(key, msg_buf, ss);
			key[ss] = '\0';
			msg_buf += ss;
			ks = ntohl(*(int *) msg_buf);
			msg_buf += _INT;
			if ((d = data_create(ks)) == NULL){
				free(key);
				free(m);
				return NULL;
			}
			base64_decode(msg_buf, ks, d->data, &ks);
			//memcpy(d->data, msg_buf, ks);
			m->content.entry = (struct entry_t *)malloc(sizeof(struct entry_t));
			if (m->content.entry == NULL){
				data_destroy(d);
				free(key);
				free(m);
				return NULL;
			}
			m->content.entry->key = strdup(key);
			if (m->content.entry->key == NULL){
				data_destroy(d);
				free(key);
				free(m->content.entry);
				free(m);
				return NULL;
			}
			m->content.entry->value = data_dup(d);
			if (m->content.entry->value == NULL){
				data_destroy(d);
				free(key);
				free(m->content.entry->key);
				free(m->content.entry);
				free(m);
				return NULL;
			}
			data_destroy(d);
			free(key);
			
			return m;
		case CT_NONE:
			return m;
	}
	free(m);
	return NULL;
}

