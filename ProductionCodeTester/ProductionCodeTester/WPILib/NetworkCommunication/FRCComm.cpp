
#include <VxWorks.h>
#include "FRCComm.h"
#include <Simulator/Simulator.h>


int getControlData(FRCControlData *data, char *userData, int wait_ms)
{
	return Simulator::GetInstance()->GetControlData(data, userData);
}

int setStatusData(float battery, UINT8 dsDigitalOut, const char *userData, int userDataLength, int wait_ms)
{
	return 0;
}

int setUserDsLcdData(const char *userDsLcdData, int userDsLcdDataLength, int wait_ms)
{
	Simulator::GetInstance()->SetLCDData(userDsLcdData, userDsLcdDataLength);
	return 0;
}

void setNewDataSem(SEM_ID)
{
}

void setResyncSem(SEM_ID)
{
}

void signalResyncActionDone(void)
{
}

// this UINT32 is really a LVRefNum
void setNewDataOccurRef(UINT32 refnum)
{
}

void setResyncOccurRef(UINT32 refnum)
{
}

void FRC_NetworkCommunication_getVersionString(char *version)
{
}
