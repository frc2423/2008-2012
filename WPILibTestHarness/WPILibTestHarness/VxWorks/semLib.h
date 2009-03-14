
#ifndef EMULATION_SEMLIB_H
#define EMULATION_SEMLIB_H

#include "VxWorks.h"
#include <Simulator/Simulator.h>

#define SEM_DELETE_SAFE 		1
#define SEM_INVERSION_SAFE 		2
#define SEM_Q_PRIORITY 			4

#define SEM_FULL				0
#define SEM_EMPTY				1

#define WAIT_FOREVER -1



static inline SEM_ID semBCreate(int semId, int start)
{
	return start;
}

static inline
SEM_ID semMCreate(int options)
{
	return OK;
}

static inline
int semDelete(SEM_ID semId)
{
	return OK;
}

static inline
int semGive(SEM_ID &semId) 
{
	assert(semId);
	semId = 0;
	return OK; 
}

static inline
int semTake(SEM_ID &semId, int timeout) 
{ 
	// this only works because we don't do multiple threads!
	if (timeout == 0)
	{
		if (semId == 1)
			return -1;
			
		semId = 1;
		return OK;
	}

	// todo: support different values
	assert(timeout == WAIT_FOREVER);
	
	while (semId) Simulator::Wait(0.01);
	semId = 1;
	return 0; 
}

static inline
void semFlush(SEM_ID &semId)
{
	semId = 0;
}

#endif
