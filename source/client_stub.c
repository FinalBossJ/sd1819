

#include "client_stub.h"
#include "client_stub-private.h"
#include "table.h"

/* Função para estabelecer uma associação entre o cliente e uma tabela no 
 * servidor.
 * address_port é uma string no formato <hostname>:<port>.
 * retorna NULL em caso de erro
 */
struct rtable_t *rtable_connect(const char *address_port){

	if(address_port==NULL)
		return NULL;
	struct rtable_t * rtable = (struct rtable_t*) malloc(sizeof(struct rtable_t));
	if(rtable==NULL)
		return NULL;
	
	struct server_t * server = network_connect(address_port);
	if(server==NULL)
		return NULL;

	rtable->server = server;
	return rtable;

}

/* Termina a associação entre o cliente e a tabela remota, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_disconnect(struct rtables_t *rtable){
	
	if(rtable==NULL)
		return -1;
	if(network_close(rtable->server) != 0)
		return -1;
	free(rtable);
	return 0;

}

/* Funcao para adicionar um elemento na tabela.
 * Se a key ja existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry){

	if(rtable == NULL || entry == NULL )
		return -1;
	struct message_t * msg = malloc(sizeof(struct message_t));
	msg->opcode = OP_PUT;
	msg->c_type = CT_ENTRY;
	if((msg->content.entry = (struct entry_t*) malloc(sizeof(struct entry_t))) == NULL) {
        free_message(msg);
        return -1;
	}
	msg->content.entry->key = key;
	msg->content.entry->value = value;

	printf("\nA ENVIAR:\n");
    print_message(msg);

	struct message_t* message_received = network_send_receive(rtable->server, msg);
	
    if(message_received != NULL) {
        printf("A RECEBER:\n");
        print_message(message_received);
        return 0;
    }
	return -1;
}

/* Funcao para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key){
	if(rtable == NULL || key == NULL )
		return NULL;

	struct message_t* msg = (struct message_t*) malloc(sizeof(struct message_t));
	if (msg == NULL)
		return NULL;
	msg->opcode = OP_GET;
	msg->c_type = CT_KEY;
	msg->content.key = strdup(key);
	printf("\nA ENVIAR:\n");
    print_message(msg);

	struct message_t* message_received = network_send_receive(rtable->server, msg);
	
    if(message_received != NULL) {
        printf("A RECEBER:\n");
        print_message(message_received);
        return message_received->content.data;
    }
	return NULL;

}

/* Funcao para remover um elemento da tabela. Vai libertar 
 * toda a memoria alocada na respectiva operacao rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key);

/* Devolve o número de elementos da tabela.
 */
int rtable_size(struct rtable_t *rtable){
	if(rtable = NULL)
		return -1;

	struct message_t* msg = (struct message_t*) malloc(sizeof(struct message_t));
	msg->opcode = OP_SIZE;
	msg->c_type = CT_NONE;

	printf("\nA ENVIAR:\n");
    print_message(msg);

	struct message_t* message_received = network_send_receive(rtable->server, msg);
	
    if(message_received != NULL) {
        printf("A RECEBER:\n");
        print_message(message_received);
        return (int)message_received->content.result;
    }
    return -1;
}
	

/* Devolve um array de char* com a copia de todas as keys da tabela,
 * colocando um ultimo elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable){
	if(rtable == NULL)
		return NULL;

	struct message_t* msg = (struct message_t*) malloc(sizeof(struct message_t));
	msg->opcode = OP_GET;
	msg->c_type = CT_NONE;

	printf("\nA ENVIAR:\n");
    print_message(msg);

	struct message_t* message_received = network_send_receive(rtable->server, msg);
	
    if(message_received != NULL) {
        printf("A RECEBER:\n");
        print_message(message_received);
    }
	if (message_received->opcode != OP_GET + 1) {
		free(msg);
		free_message(message_received);
		return NULL;
	}

	int nKeys = 0;
	while(message_received->content.keys[nKeys] != NULL)
		nKeys++;

	char** keyCopy = malloc(sizeof(nKeys + 1));
	int i = 0;
	while( i < nKeys){
		keyCopy[i] = strdup(message_received->content.keys[i]);
		i++;
	}
	keyCopy[i] = NULL;
	return keyCopy;
	
}

/* Liberta a memoria alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys){

	int i = 0;
	while(keys[i] != NULL){
		free(keys[i]);
		i++;
	}
	free(keys);

}