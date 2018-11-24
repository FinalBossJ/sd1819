/*
SD 2018/2019
Projecto 3 - Grupo 32
Sandro Correia - 44871
Diogo Catarino - 44394
Pedro Almeida - 46401
*/
#ifndef _PERSISTENT_TABLE_PRIVATE_H
#define _PERSISTENT_TABLE_PRIVATE_H

#include "persistent_table.h"
#include "persistence_manager-private.h"

struct ptable_t{
	struct pmanager_t *pmanager;
	struct table_t *table;
};
#endif