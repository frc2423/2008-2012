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

#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H

#include <vector>
#include <VxWorks/VxWorks.h>

#include "WheelSimulator.h"

struct FRCControlData;



struct ControlInterface;
class SimulationWindow;

/// provided to decouple the GUI related code from the simulation code
class Simulator {
public:

	// singleton instance of this class
	static Simulator * m_instance;	
	
	/// \name API For Simulation data
	/// @{
	
	static bool ShouldContinue();

	// step the simulation up a period of time (in seconds)
	static void Wait(double tm){ NextStep(tm); }
	static void NextStep(double tm) { m_instance->SimulateStep(tm); }

	// get time for WPILib
	static double GetTime() { 
		if (Simulator::m_instance)
			return Simulator::m_instance->m_time; 
		return 0;
	}
	
	// set LCD output data
	static void SetLCDData(const char * userDsLcdData, int userDsLcdDataLength);
	
	// retrieves control information
	static int GetControlData(FRCControlData *data, char *userData);
	
	static void AddEncoder(Encoder * e, UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel);
	static void DeleteEncoder(Encoder * e);
	
	static void AddPWM(PWM * p, UINT32 slot, UINT32 channel);
	static void DeletePWM(PWM * p);
	
	static void AddGyro(Gyro * g, UINT32 slot, UINT32 channel);
	static void DeleteGyro(Gyro * g);
	
	static void AddNotifier(Notifier * n);
	static void DeleteNotifier(Notifier * n);
	
	static void AddDigitalInput(DigitalInput * di, UINT32 slot, UINT32 channel);
	static void DeleteDigitalInput(DigitalInput * di);
	
	/// @}
	
	/// \name Simulation functions
	/// @{
	
	static void StartSimulation(ControlInterface * controlInterface);
	
	/// @}
	
private:

	Simulator();
	Simulator(ControlInterface * controlInterface);
	
	void SimulateStep(double tm);
	
	static void FillDigitalIoSlot(UINT32 slot, UINT32 channel);
	
	std::vector <Notifier*> 		m_notifiers;
	
	// current time
	double 							m_time;
	ControlInterface *				m_controlInterface;
};


#endif
