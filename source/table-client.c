/*
SD 2018/2019
Projecto 3 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "client_stub-private.h"
#include "network_client-private.h"

/*
	Programa cliente para manipular tabela de hash remota.

	Uso: table-client <ip servidor>:<porta servidor>
	Exemplo de uso: ./table_client 10.101.148.144:54321
*/

int main(int argc, char **argv){
	struct rtable_t *rtable;

	char input[81];
    char* address_port;

    //Usar a função signal() para ignorar sinais do tipo SIGPIPE
    signal(SIGPIPE, SIG_IGN);

	/* Testar os argumentos de entrada */
	if (argc != 2){
        printf("Uso: ./table_client <ip_servidor>:<porto_servidor>\n");
        printf("Exemplo de uso: binary/table-client 127.0.0.1:54322\n");
        return -1;
    }

    //Definir endereco do servidor
    address_port = strdup(argv[1]);

	/* Iniciar instância do stub e Usar rtable_connect para estabelcer ligação ao servidor*/

	rtable = rtable_connect(address_port);

	//Verificar se a associacao foi bem estabelecida
    if(rtable == NULL){
        printf("Ocorreu um erro no estabelecimento da associacao entre o cliente e a tabela remota.\n");
        return -1;
    }

	/* Fazer ciclo até que o utilizador resolva fazer "quit" */
 	while (1){

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador*/
 		fgets(input,81,stdin);
        
        int i = 0;
        while(input[i] != '\n') 
            i++;
        input[i] = '\0';


		/* Verificar se o comando foi "quit". Em caso afirmativo
		   não há mais nada a fazer a não ser terminar decentemente.
		*/
        if (strcmp(input, "quit") == 0) {
            break;
        }
		
		/* Caso contrário:
			Verificar qual o comando;	
			chamar função do stub para processar o comando e receber msg_resposta
		*/
		char* args = strdup(input);
        int nArgs = 0;
        if(strtok(args, " ") != NULL){
            while((strtok(NULL, " ")) != NULL){
                nArgs++;
            }
           

        char* command = strtok(input, " ");


        //Processar o pedido do utilizador
        //put <key> <data>
        if(strncmp(command, "put", 3) == 0 && nArgs >= 2) {
            struct data_t *value;
            struct entry_t *entry;
            char *key = strtok(NULL, " ");
            char* data = strtok(NULL, "");
            if((value = data_create2(sizeof(data), data)) == NULL) {
                free(value);
                break;
            }

            if((entry = entry_create(key, value)) == NULL){
            	data_destroy(value);
            	break;
            }

            if(rtable_put(rtable, entry) == -1){
                printf("Tentando reconectar ao servidor...\n");
                sleep(RETRY_TIME);
                rtable = rtable_connect(address_port);
                //Verificar se a associacao foi bem estabelecida
                if(rtable == NULL){
                    printf("Ocorreu um erro no estabelecimento da associacao entre o cliente e a tabela remota.\n");
                    break;
                }
                printf("Reconexao ao servidor efectuada com sucesso!\n");
                if(rtable_put(rtable, entry) == -1){
                    entry_destroy(entry);
                    break;
                }
            }

        //get <key>  
        }else if(strncmp(command, "get", 3) == 0 && nArgs == 1) {
            char *key = strtok(NULL, " "); 
            if(rtable_get(rtable, key) == NULL){
                printf("Tentando reconectar ao servidor...\n");
                sleep(RETRY_TIME);
                rtable = rtable_connect(address_port);
                //Verificar se a associacao foi bem estabelecida
                if(rtable == NULL){
                    printf("Ocorreu um erro no estabelecimento da associacao entre o cliente e a tabela remota.\n");
                    break;
                }
                printf("Reconexao ao servidor efectuada com sucesso!\n");
                if(rtable_get(rtable, key) == NULL){
                    break;
                }
            }

        //del <key>  
        }else if(strncmp(command, "del", 3) == 0 && nArgs == 1) {
            char *key = strtok(NULL, " "); 
            if(rtable_del(rtable, key) == -1){
                printf("Tentando reconectar ao servidor...\n");
                sleep(RETRY_TIME);
                rtable = rtable_connect(address_port);
                //Verificar se a associacao foi bem estabelecida
                if(rtable == NULL){
                    printf("Ocorreu um erro no estabelecimento da associacao entre o cliente e a tabela remota.\n");
                    break;
                }
                printf("Reconexao ao servidor efectuada com sucesso!\n");
                if(rtable_del(rtable, key) == -1){
                    break;
                }
            }

        //size
        } else if(strncmp(command, "size", 4) == 0 && nArgs == 0) {
            if(rtable_size(rtable)==-1){
                printf("Tentando reconectar ao servidor...\n");
                sleep(RETRY_TIME);
                rtable = rtable_connect(address_port);
                //Verificar se a associacao foi bem estabelecida
                if(rtable == NULL){
                    printf("Ocorreu um erro no estabelecimento da associacao entre o cliente e a tabela remota.\n");
                    break;
                }
                printf("Reconexao ao servidor efectuada com sucesso!\n");
                if(rtable_size(rtable)==-1){
                    break;
                }
            }
            
        //getkeys
        }else if(strncmp(command, "getkeys", 7) == 0 && nArgs == 0) { 
            char **keys;
            if((keys = rtable_get_keys(rtable)) == NULL){
                printf("Tentando reconectar ao servidor...\n"); 
                sleep(RETRY_TIME);
                rtable = rtable_connect(address_port);
                //Verificar se a associacao foi bem estabelecida
                if(rtable == NULL){
                    printf("Ocorreu um erro no estabelecimento da associacao entre o cliente e a tabela remota.\n");
                    break;
                }
                printf("Reconexao ao servidor efectuada com sucesso!\n");
                if((keys = rtable_get_keys(rtable)) == NULL){
                   break;
                }
            }


        }else{ 
            printf("\nInputs reconhecidos sao:\n");
            printf("put <key> <data>\n");
            printf("get <key>\n");
            printf("del <key>\n");
            printf("size\n");
            printf("getkeys\n");
            printf("quit\n\n");
        }
        printf("\n######################\n");
        printf("######################\n");
        }
    }

  	return rtable_disconnect(rtable);
}

