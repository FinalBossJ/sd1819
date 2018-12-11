/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "message-private.h"
#include "network_client-private.h"
#include "client_stub-private.h"

/* Esta função deve:
* - Obter o endereço do servidor (struct sockaddr_in) a base da
* informação guardada na estrutura rtable;
* - Estabelecer a ligação com o servidor;
* - Guardar toda a informação necessária (e.g., descritor do socket)
* na estrutura rtable;
* - Retornar 0 (OK) ou -1 (erro).
*/
int network_connect(struct rtable_t *rtable){
    if(rtable == NULL)
        return -1;
    
    struct server_t* server = rtable->server;

    if(server == NULL)
    	return -1;

    server->inf = malloc(sizeof(struct sockaddr_in));
            
    char * new_address_port = strdup(server->address);
    char *hostname;
    char *port;
    
    hostname = strtok(new_address_port, ":");
    port = strtok(NULL, " ");

    if (port == NULL || (server->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Erro ao criar socket TCP");
            free(server->inf);
            free(server);
            return -1;
    }

    server->inf->sin_family = AF_INET;
    server->inf->sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, hostname, &(server->inf->sin_addr)) < 1) {
        perror("Erro ao converter IP");
        network_close(rtable);
        return -1;
    }
    
    if (connect(server->socket,(struct sockaddr *)(server->inf), sizeof(struct sockaddr_in)) < 0) {
        perror("Erro ao conectar-se ao servidor");
        network_close(rtable);
        return -1;
    }
  
    return 0;
}

/* Esta função deve:
* - Obter o descritor da ligação (socket) da estrutura rtable_t;
* - Serializar a mensagem contida em msg;
* - Enviar a mensagem serializada para o servidor;
* - Esperar a resposta do servidor;
* - De-serializar a mensagem de resposta;
* - Retornar a mensagem de-serializada ou NULL em caso de erro.
*/
struct message_t *network_send_receive(struct rtable_t * rtable, struct message_t *msg){
	char *message_out, *buffer_in;
	int message_size, msg_size, result;
	struct message_t* msg_resposta;
    
	/* Verificar parâmetros de entrada */
	if(msg == NULL || rtable == NULL){
		return NULL;
	}

	struct server_t* server = rtable->server;

    if(server == NULL)
    	return NULL;

	/* Serializar a mensagem recebida */
    
	message_size = message_to_buffer(msg, &message_out);

    /* Verificar se a serialização teve sucesso */
    
    if (message_size <= 0) {
        printf("Erro serializacao de msg\n");
        free_message(msg);
        free(message_out);
		return NULL;
    }

	/* Enviar ao servidor o tamanho da mensagem que será enviada
	   logo de seguida
	*/

	msg_size = htonl(message_size);
 	result = write_all(server->socket, (char *) &msg_size, _INT);
	/* Verificar se o envio teve sucesso */
	if (sizeof(msg_size) != result){
        printf("Erro no write_all(tamanho da msg)\n");
        free(message_out);
        return NULL;
        
    }

	/* Enviar a mensagem que foi previamente serializada */
	result = write_all(server->socket, message_out, message_size);

	/* Verificar se o envio teve sucesso */
	if (message_size != result){
        printf("Erro no write_all(msg)\n");
        free(message_out);
        return NULL;
    }

    free(message_out);

	/* De seguida vamos receber a resposta do servidor: */
	/* Com a função read_all, receber num inteiro o tamanho da
		mensagem de resposta.*/
	result = read_all(server->socket, (char *) &msg_size, _INT);
    if (sizeof(msg_size) != result){
    	printf("Erro no read_all(tamanho da msg)\n");
        return NULL;
    }

    /* Alocar memória para receber o número de bytes da
		mensagem de resposta. */
	message_size = ntohl(msg_size);
    buffer_in = (char *) malloc (message_size);
    if (buffer_in == NULL) return NULL;

	/*	Com a função read_all, receber a mensagem de resposta. */
	result = read_all(server->socket, buffer_in, message_size);
	if (message_size != result){
		printf("Erro no read_all(msg)\n");
        return NULL;
    }

	/* Desserializar a mensagem de resposta */
	msg_resposta = buffer_to_message(buffer_in, message_size);

	/* Verificar se a desserialização teve sucesso */
	if(msg_resposta == NULL) {
        printf("Mensagem nula recebida\n");
        free(buffer_in);
        return NULL;
    }
	/* Libertar memória */
	free(buffer_in);
	return msg_resposta;
}


/* A função network_close() fecha a ligação estabelecida por
* network_connect().
*/
int network_close(struct rtable_t * rtable){
	if (rtable == NULL)
		return -1;

	struct server_t* server = rtable->server;

    if(server == NULL)
    	return -1;
	/* Terminar ligação ao servidor */
    close(server->socket);

	/* Libertar memória */
	free(server->inf);
	free(server);

	return 0;
}
