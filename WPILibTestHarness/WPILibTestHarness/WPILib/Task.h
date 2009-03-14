/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef TASK_H
#define TASK_H

#include "Base.h"
#include <vxworks.h>

/**
 * WPI task is a wrapper for the native Task object.
 * All WPILib tasks are managed by a static task manager for simplified cleanup.
 **/
class Task
{
public:
	static const UINT32 kDefaultPriority = 100;
	static const INT32 kInvalidTaskID = -1;

	Task(char* name, FUNCPTR function, INT32 priority = kDefaultPriority, UINT32 stackSize = 20000){
		m_taskName = _strdup(name);
	}
	virtual ~Task(){
		free(m_taskName);
	}

	bool Start(UINT32 arg0 = 0, UINT32 arg1 = 0, UINT32 arg2 = 0, UINT32 arg3 = 0, UINT32 arg4 = 0, 
			UINT32 arg5 = 0, UINT32 arg6 = 0, UINT32 arg7 = 0, UINT32 arg8 = 0, UINT32 arg9 = 0){ return true; }
	bool Restart(void) { return true; }
	bool Stop(void) { return true; }

	bool IsReady(void) { return true; }
	bool IsSuspended(void) { return false; }

	bool Suspend(void) { return true; }
	bool Resume(void) { return true; }

	bool Verify(void) { return true; }

	INT32 GetPriority(void) { return kDefaultPriority; }
	bool SetPriority(INT32 priority) { return true; }
	char* GetName(void) { return m_taskName; }
	INT32 GetID(void) { return 42; }

private:
	FUNCPTR m_function;
	char* m_taskName;
	INT32 m_taskID;
	UINT32 m_stackSize;
	INT32 m_priority;
	bool HandleError(STATUS results);
	DISALLOW_COPY_AND_ASSIGN(Task);
};

#endif
