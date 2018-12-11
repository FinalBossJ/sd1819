/*
SD 2018/2019
Projecto 4 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "client_stub-private.h"
#include "network_client-private.h"

//*************************************************************
//Variaveis globais
char* address_port;
int nOp = 0; //Numero total de operacoes concluidas pelo conjunto de threads
long totLat = 0; //Latencia total de todas as operacoes concluidas (totLat/nOp = latencia media de cada operacao concluida)
char* mode; //Modo automatico (p para escrita; g para leitura)

pthread_mutex_t dados = PTHREAD_MUTEX_INITIALIZER;
//*************************************************************

void* thread_main(void* params){
    struct rtable_t *rtable;
    /* Iniciar instância do stub e Usar rtable_connect para estabelcer ligação ao servidor*/
    pthread_mutex_lock(&dados);
    rtable = rtable_connect(address_port);
    pthread_mutex_unlock(&dados);

    //Verificar se a associacao foi bem estabelecida
    if(rtable == NULL){
        return (void *) -1;
    }

    
  //ciclo da thread:
  while(1){
    pthread_mutex_lock(&dados);
    struct timeval before, after;
    
            if(strcmp(mode, "p") == 0){
                nOp++;
                struct data_t *value;
                struct entry_t *entry;
                char* key = (char *)malloc(sizeof(int)+1);
                sprintf(key, "%d", nOp);
                char* data = (char *)malloc(sizeof(int)+1);
                sprintf(data, "%d", getpid());
                if((value = data_create2(sizeof(data), data)) == NULL) {
                    free(value);
                    return (void *)-1;
                }

                if((entry = entry_create(key, value)) == NULL){
                    data_destroy(value);
                    return (void *)-1;
                }
                gettimeofday(&before, 0);
                if(rtable_put(rtable, entry) == -1){
                    entry_destroy(entry);
                    return (void *)-1;
                }
                gettimeofday(&after, 0);
            }else if(strcmp(mode, "g") == 0){
                nOp++;
                char* key = (char *)malloc(sizeof(int)+1);
                sprintf(key, "%d", nOp);
                gettimeofday(&before, 0);
                if(rtable_get(rtable, key) == NULL){
                    return (void *)-1;
                }
                gettimeofday(&after, 0);
            }
    totLat = (after.tv_sec-before.tv_sec)*1000000 + after.tv_usec-before.tv_usec;
    pthread_mutex_unlock(&dados);
  }
  return 0;
}

/*
	Programa cliente para manipular tabela de hash remota.

	Uso: table-client <ip servidor>:<porta servidor>
	Exemplo de uso: ./table_client 10.101.148.144:54321
*/

int main(int argc, char **argv){
	struct rtable_t *rtable;

	char input[81];

    //Usar a função signal() para ignorar sinais do tipo SIGPIPE
    signal(SIGPIPE, SIG_IGN);

	/* Testar os argumentos de entrada */
	if (argc != 2 && argc != 5){
        printf("Uso: ./table_client <ip_servidor>:<porto_servidor>\n");
        printf("Modo automatico de escrita: ./table_client <ip_servidor>:<porto_servidor> p <secs> <nthreads>\n");
        printf("Modo automatico de leitura: ./table_client <ip_servidor>:<porto_servidor> g <secs> <nthreads>\n");
        printf("Exemplo de uso: binary/table-client 127.0.0.1:54322\n");
        return -1;
    }

    //Definir endereco do servidor
    address_port = strdup(argv[1]);


    //Caso o cliente esteja em modo automatico
    if(argc != 2){
        mode = argv[2];
        if(strcmp(mode, "g") != 0 && strcmp(mode, "p") != 0){
            printf("Modo automatico invalido \"%s\".\n", mode);
        }
        int nThreads = atoi(argv[4]);
        if(nThreads == 0){
            printf("Numero de threads invalido.\n");
            return -1;
        }
        pthread_t threads[nThreads];
        for(int i = 0; i <  nThreads; i++){
            if (pthread_create(&threads[i], NULL, &thread_main, (void*)address_port ) != 0){
                printf("Ocorreu um erro na criacao das threads.\n");
                return -1;
            }
        }
        sleep(atoi(argv[3]));
        for(int j = 0; j < nThreads; j++){
            if(pthread_detach(threads[j]) != 0){
                printf("Ocorreu um erro ao terminar as threads.\n");
                return -1;
            }
        }
        printf("Numero total de operacoes concluidas pelo conjunto de threads: %d\n", nOp);
        printf("Latencia total: %ld microsegundos\n", totLat);
        printf("Latencia media de cada operacao concluida: %ld microsegundos\n", totLat/nOp);
        return 0;
    }

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

