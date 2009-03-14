/*
    WPILib Test Harness
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License v3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SIMULATOR_CONTROLINTERFACE_H
#define SIMULATOR_CONTROLINTERFACE_H

#include <wx/wx.h>

#include <VxWorks/VxWorks.h>
#include <WPILib/NetworkCommunication/FRCComm.h>

#include "SimulationData.h"

DECLARE_EVENT_TYPE(EVT_ON_STEP, -1)

// 10ms default step size
#define SIMULATOR_STEP_SIZE 0.01
#define SIMULATOR_STEP_TIMER 10


struct ControlInterface {

	// if 0, then the simulation stops
	wxSemaphore 	signal;

	// this is the global lock for any shared simulation variables
	wxMutex			lock;
	
	// set this to true if simulation should exit
	bool			exit_now;
	
	wxWindow *		evtHandler;
	
	// driver station data going to the simulation
	FRCControlData	controlData;
	
	// hardware data going back and forth
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
