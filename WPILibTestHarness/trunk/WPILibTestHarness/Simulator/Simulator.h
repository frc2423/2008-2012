/*
    WPILib Test Harness
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>
	
	$Id$

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

struct FRCControlData;

class Encoder;
class Gyro;
class PWM;
class DigitalInput;
class DigitalOutput;
class AnalogChannel;
class Solenoid;

class Notifier;

struct ControlInterface;
class SimulationWindow;

/// provided to decouple the GUI related code from the simulation code
class Simulator {
public:

	static Simulator * GetInstance()
	{
		return Simulator::m_instance;
	}
	
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
	void SetLCDData(const char * userDsLcdData, int userDsLcdDataLength);
	
	// retrieves control information
	int GetControlData(FRCControlData *data, char *userData);
	
	void AddEncoder(Encoder * e, UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel);
	void DeleteEncoder(Encoder * e);
	
	void AddPWM(PWM * p, UINT32 slot, UINT32 channel);
	void DeletePWM(PWM * p);
	
	void AddGyro(Gyro * g, UINT32 slot, UINT32 channel);
	void DeleteGyro(Gyro * g);
	
	void AddNotifier(Notifier * n);
	void DeleteNotifier(Notifier * n);
	
	void AddDigitalInput(DigitalInput * di, UINT32 slot, UINT32 channel);
	void DeleteDigitalInput(DigitalInput * di);
	
	void AddDigitalOutput(DigitalOutput * d, UINT32 slot, UINT32 channel);
	void DeleteDigitalOutput(DigitalOutput * d);
	
	void AddAnalogChannel(AnalogChannel * ac, UINT32 slot, UINT32 channel);
	void DeleteAnalogChannel(AnalogChannel * ac);
	
	void AddSolenoid(Solenoid * s, UINT32 slot, UINT32 channel);
	void DeleteSolenoid(Solenoid * s);
	
	/// @}
	
	/// \name Simulation functions
	/// @{
	
	static void StartSimulation(ControlInterface * controlInterface);
	
	/// @}
	
private:

	Simulator();
	Simulator(ControlInterface * controlInterface);
	
	// singleton instance of this class
	static Simulator * m_instance;
	
	void SimulateStep(double tm);
	
	void FillDigitalIoSlot(UINT32 slot, UINT32 channel);
	void FreeDigitalIoSlot(UINT32 slot, UINT32 channel);
	void FillAnalogIoSlot(UINT32 slot, UINT32 channel);
	void FreeAnalogIoSlot(UINT32 slot, UINT32 channel);
	
	std::vector <Notifier*> 		m_notifiers;
	
	// current time
	double 							m_time;
	ControlInterface *				m_controlInterface;
};


#endif
