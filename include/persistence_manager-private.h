/*
SD 2018/2019
Projecto 3 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#ifndef _PERSISTENCE_MANAGER_PRIVATE_H
#define _PERSISTENCE_MANAGER_PRIVATE_H

#include "persistence_manager.h"

struct pmanager_t{
	const char *  logname;
	const char *  ckpname;
	const char *  sttname;
	int logsize;
};
#endif