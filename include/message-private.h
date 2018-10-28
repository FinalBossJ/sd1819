/*
SD 2018/2019
Projecto 2 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/

#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include <signal.h>
#include "message.h"

#define _SHORT 2
#define _INT 4


/* Função que garante o envio de len bytes armazenados em buf,
   através da socket sock.
*/
int write_all(int sock, char *buf, int len);

/* Função que garante a receção de len bytes através da socket sock,
   armazenando-os em buf.
*/
int read_all(int sock, char *buf, int len);

//Imprime uma message
void print_message(struct message_t *msg);

#endif