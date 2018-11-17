/*
SD 2018/2019
Projecto 3 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/

#define NFDESC 4 // Número de sockets (uma para listening)

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#include "message-private.h"
#include "table_skel.h"
#include "network_server.h"

int global_socket;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar 0 (OK) ou -1 (erro).
 */
int network_server_init(short port){
  int socket_fd, sim;
  struct sockaddr_in server;

  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Erro ao criar socket");
    return -1;
  }

  //Permite a reutilizacao do socket
  sim = 1;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (int *)&sim, sizeof(sim)) <0)
    perror("SO_REUSEADDR setsockopt error");


  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0){
      perror("Erro ao fazer bind");
      close(socket_fd);
      return -1;
  }

  if (listen(socket_fd, 0) < 0){
      perror("Erro ao executar listen");
      close(socket_fd);
      return -1;
  }
  global_socket = socket_fd;
  return socket_fd;
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){
	int nfds, kfds, i;
	struct pollfd connections[NFDESC]; // Estrutura para file descriptors das sockets das ligações
	struct message_t* msg;
	struct sockaddr_in client;
  	socklen_t size_client;
  	printf("Server up...\n");

	for (i = 0; i < NFDESC; i++)
    	connections[i].fd = -1;    // poll ignora estruturas com fd < 0
	connections[0].fd = listening_socket;  // Vamos detetar eventos na welcoming socket
	connections[0].events = POLLIN;  // Vamos esperar ligações nesta socket

	nfds = 1; // número de file descriptors

	static int pool[NFDESC]; //pool de sockets
	pool[0] = 1; //listening socket esta sempre reservado



  // Retorna assim que exista um evento ou que TIMEOUT expire. * FUNÇÂO POLL *.
  while ((kfds = poll(connections, NFDESC, 10)) >= 0) // kfds == 0 significa timeout sem eventos
    if (kfds > 0){
      
      if ((connections[0].revents & POLLIN) && (nfds < NFDESC)){  // Pedido na listening socket ?
  
        //Arranjar uma socket livre da pool
       int openSocket = 0;
        for(i = 1; i < NFDESC && openSocket == 0; i++){
          if(pool[i] == 0){
            openSocket = i;
          }
        }
        if ((connections[openSocket].fd = accept(connections[0].fd, (struct sockaddr *) &client, &size_client)) > 0){ // Ligação feita ?
          connections[openSocket].events = POLLIN; // Vamos esperar dados nesta socket
          nfds++;
          pool[openSocket] = 1;

        }
      }
      for (i = 1; i < NFDESC; i++){// Todas as ligações
        //Se a socket estiver a ser utilizada
        if(pool[i] == 1){
        if (connections[i].revents & POLLIN) { //Dados para ler?
          
          
              if ((msg = network_receive(connections[i].fd)) == NULL) {
                  printf("Ligacao terminada com um cliente.\n");
                  close(connections[i].fd);
                  connections[i].fd = -1;
                  nfds--;
                  pool[i] = 0;
              }else if((network_send(connections[i].fd, msg)) == -1){
              	printf("Ligacao terminada com um cliente.\n");
                  close(connections[i].fd);
                  connections[i].fd = -1;
                  nfds--;
                  pool[i] = 0;
              }
            
        }
      }
      }
    }
    for (i = 1; i < nfds; i++){
      close(connections[i].fd);
      connections[i].fd = -1;
    }

	printf("Closing...\n");
	network_server_close();	
	return 0;	
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
	char *message_pedido;
	int message_size, msg_size, result;
	struct message_t *msg_pedido;


	/* Verificar parâmetros de entrada */
	if (client_socket < 0){
		printf("Estrutura de entrada invalida");
		return NULL;
	}

	//Verificar se o socket ainda esta aberto
	char buffer[32];
	if(recv(client_socket, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0){
		return NULL;
	}
	/* Com a função read_all, receber num inteiro o tamanho da
	 mensagem de pedido que será recebida de seguida.*/
	result = read_all(client_socket, (char *) &msg_size, _INT);

	/* Verificar se a receção teve sucesso */
	if(result==0 || result != _INT){
		printf("Erro: Mensagem com tamanho incorreto\n");
		return NULL;
	}

	message_size = ntohl(msg_size);

	/* Alocar memória para receber o número de bytes da
	 mensagem de pedido. */
	message_pedido = (char *) malloc(message_size);
	if(message_pedido == NULL)
		return NULL;

	/* Com a função read_all, receber a mensagem de resposta. */
	result = read_all(client_socket, message_pedido, message_size);

	/* Verificar se a receção teve sucesso */
	if(message_pedido==NULL || result != message_size){
		printf("message pedido null\n");
		free(message_pedido);
		return NULL;
	}

	/* Desserializar a mensagem do pedido */
	msg_pedido = buffer_to_message(message_pedido, message_size);

	/* Verificar se a desserialização teve sucesso */
	if(msg_pedido==NULL){
		printf("Erro: Falha na desserializacao da mensagem de pedido\n");
		return NULL;
	}
	free(message_pedido);

	printf("\n\n######################\n");
    printf("######################\n\n\n");
    printf("Mensagem recebida do cliente: \n");
    print_message(msg_pedido);
	return msg_pedido;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg){
	/* Verificar parâmetros de entrada */
	if (client_socket < 0){
		printf("Erro: Estrutura de entrada invalida");
		return -1;
	}

	if(msg == NULL || invoke(msg) == -1) //Where the magic is done
		return -1;

	//Verificar se o socket ainda esta aberto
	char buffer[32];
	if(recv(client_socket, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0){
		return -1;
	}

	printf("\n\n######################\n");
    printf("######################\n\n\n");
	printf("Mensagem enviada para o cliente: \n");
	print_message(msg);

	/* Serializar a mensagem recebida */

	char *message_resposta;
	int message_size = message_to_buffer(msg, &message_resposta);

	free_message(msg);

	/* Verificar se a serialização teve sucesso */
	if(message_size==0){
		printf("Erro: Falha na serializacao da mensagem de resposta");
		return -1;
	}

	/* Enviar ao cliente o tamanho da mensagem que será enviada
	logo de seguida
	*/
	int msg_size = htonl(message_size);
	int result = write_all(client_socket, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
	if(result != _INT){
		printf("Erro: Envio de tamanho de msg resposta");
		free(message_resposta);
		return -1;
	}

	/* Enviar a mensagem que foi previamente serializada */
	result = write_all(client_socket, message_resposta, message_size);

	/* Verificar se o envio teve sucesso */
	if(result != message_size){
		printf("Erro: Envio de mensagem resposta falhou");
		free(message_resposta);
		return -1;
	}
	return 0;
}

/* A função network_server_close() fecha a ligação estabelecida por
 * network_server_init().
 */
int network_server_close(){
	close(global_socket);
	return 0;
}
