
#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include <wx/wx.h>

#include <VxWorks/VxWorks.h>
#include <WPILib/NetworkCommunication/FRCComm.h>

#include "SimulationData.h"

DECLARE_EVENT_TYPE(EVT_ON_STEP, -1)

// 25ms default step size
#define SIMULATOR_STEP_SIZE 0.025




struct ControlInterface {

	// if 0, then the simulation stops
	wxSemaphore 	signal;

	// this is the global lock for any shared simulation variables
	wxMutex			lock;
	
	// set this to true if simulation should exit
	bool			exit_now;
	
	wxWindow *		evtHandler;
	
	// data going to the simulation
	FRCControlData	controlData;
	
	// data coming from the simulation
	SimulationData	simulationData;
	
	ControlInterface(wxWindow * handler) :
		exit_now(false), evtHandler(handler)
	{
		memset(&controlData, 0, sizeof(FRCControlData));
	}
	
	void Step()
	{
		wxMutexLocker mtx(lock);
		signal.Post();
	}
	
	void Kill()
	{
		// aquire the mutex and tell the simulation to exit
		wxMutexLocker mtx(lock);
		controlData.enabled = false;
		exit_now = true;
		
		// trigger it once
		signal.Post();
	}
	
};

#endif
