
#include <VxWorks.h>
#include "FRCComm.h"
#include <Simulator/Simulator.h>

SEM_ID * packet_semaphore = NULL;

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

void setNewDataSem(SEM_ID * sem)
{
	packet_semaphore = sem;
}

void setResyncSem(SEM_ID)
{
	assert(0 && "not implemented");
}

void signalResyncActionDone(void)
{
	assert(0 && "not implemented");
}

// this UINT32 is really a LVRefNum
void setNewDataOccurRef(UINT32 refnum)
{
	assert(0 && "not implemented");
}

void setResyncOccurRef(UINT32 refnum)
{
	assert(0 && "not implemented");
}

void FRC_NetworkCommunication_getVersionString(char *version)
{
	assert(0 && "not implemented");
}

void PacketReady()
{
	if (packet_semaphore) *packet_semaphore = 0;
}
