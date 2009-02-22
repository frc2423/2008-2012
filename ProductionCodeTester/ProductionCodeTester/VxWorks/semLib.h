
#ifndef EMULATION_SEMLIB_H
#define EMULATION_SEMLIB_H

#include "VxWorks.h"

#define SEM_DELETE_SAFE 		1
#define SEM_INVERSION_SAFE 		2
#define SEM_Q_PRIORITY 			4

#define WAIT_FOREVER -1

static inline
SEM_ID semMCreate(int options)
{
	return 0;
}

static inline
int semDelete(SEM_ID semId)
{
	return 0;
}

static inline
int semGive(SEM_ID semId) { return 0; }

static inline
int semTake(SEM_ID semId, int timeout) { return 0; }


#endif
