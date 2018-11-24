/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "network_server.h"
#include "table_skel.h"

int logsize = 50;

/*
   Programa que implementa um servidor de uma tabela hash com chainning.
   Uso: table-server <port> <n_lists>
   Exemplo de uso: ./table_server 54321 6
*/

int main(int argc, char **argv){
	 int listening_socket; // file descriptor para a welcoming socket

	//Usar a função signal() para ignorar sinais do tipo SIGPIPE
	signal(SIGPIPE, SIG_IGN);

	/* Testar os argumentos de entrada */
	if (argc != 4){
		printf("Uso: ./table-server <port> <n_lists> <nome_ficheiro>\n");
		printf("Exemplo de uso: ./table-server 54321 6 servidorSeguro\n");
		return -1;
	}


	//Cria socket TCP, preenche estrutura server para bind, faz bind, liga socket aos
  	//enderecos, faz listen, marca o socket como sendo welcoming socket
	if((listening_socket = network_server_init(atoi(argv[1])))<0)
		return -1;

	int n_lists = atoi(argv[2]);
	


	if(table_skel_init(n_lists, argv[3], logsize) != -1){
		
		network_main_loop(listening_socket);

		table_skel_destroy();
	}
}