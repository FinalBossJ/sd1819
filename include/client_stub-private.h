/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#ifndef _CLIENT_STUBPRIVATE_H
#define _CLIENT_STUBPRIVATE_H

#include "client_stub.h"
#include "network_client-private.h"


struct rtable_t{
	struct server_t * server;
};

//Imprime uma message
void print_message(struct message_t *msg);

#endif