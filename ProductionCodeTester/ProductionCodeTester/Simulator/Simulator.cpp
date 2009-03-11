
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

#include "Simulator.h"
#include "ControlInterface.h"

#include <WPILib/NetworkCommunication/FRCComm.h>

#include <WPILib/DigitalInput.h>
#include <WPILib/Encoder.h>
#include <WPILib/Gyro.h>
#include <WPILib/Notifier.h>
#include <WPILib/PWM.h>

#include <WPILib/RobotBase.h>
void StartRobotClass();

#include <trunk/KwarqsLib/KwarqsConstants.h>

#include <vector>
using namespace std;

#ifdef USE_MSVC_MEMORY_TRACING
	// this enables advanced memory leak detection
	#include <wx/msw/msvcrt.h>
#endif


Simulator * Simulator::m_instance = NULL;

Simulator::Simulator(ControlInterface * controlInterface) :
	m_time(0),
	m_controlInterface(controlInterface),
	m_hardware_routed(false)
{}

// does not return until simulation is complete
void Simulator::StartSimulation(ControlInterface * controlInterface)
{
	assert(!Simulator::m_instance);
	
	// create the simulator
	Simulator::m_instance = new Simulator(controlInterface);
	StartRobotClass();
		
	// begin the simulation by calling into the user's code
	RobotBase::startRobotTask(NULL);
	
	// and then delete self when done
	delete Simulator::m_instance;
	Simulator::m_instance = NULL;
}

void Simulator::SimulateStep(double tm)
{
	{ 
		// perform calculations while protected by the lock
		wxMutexLocker mtx(m_controlInterface->lock);

		// transfer the data back and forth as needed
		m_controlInterface->simulationData.Transfer();
	
		// todo: physics calculations and such
	
	}

	// send event
	wxCommandEvent evt(EVT_ON_STEP, 42);
	m_controlInterface->evtHandler->AddPendingEvent(evt);

	// wait for next step
	if (!m_controlInterface->exit_now)
		m_controlInterface->signal.Wait();
	
	// step time before we start running stuff again
	m_time += SIMULATOR_STEP_SIZE;
	
	// finally, run the control systems code
	
	// notifiers first
	for (size_t i = 0; i < m_notifiers.size(); i++)
		m_notifiers[i]->TryEvent(m_time);

	// note: returning from this function gives control to the control loop
}

bool Simulator::ShouldContinue()
{
	if (!Simulator::m_instance || Simulator::m_instance->m_controlInterface->exit_now)
		return false;
		
	return true;
}

void Simulator::SetLCDData(const char * userDsLcdData, int userDsLcdDataLength)
{
	wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);

	if (userDsLcdDataLength > sizeof(Simulator::m_instance->m_controlInterface->simulationData.lcdText))
		userDsLcdDataLength = sizeof(Simulator::m_instance->m_controlInterface->simulationData.lcdText);
		
	memcpy(Simulator::m_instance->m_controlInterface->simulationData.lcdText, userDsLcdData, userDsLcdDataLength);
}
	
// retrieves control information
int Simulator::GetControlData(FRCControlData *data, char *userData)
{
	wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
	memcpy(data, &Simulator::m_instance->m_controlInterface->controlData, sizeof(FRCControlData));
	return 0;
}


void Simulator::FillDigitalIoSlot(UINT32 slot, UINT32 channel)
{	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		Simulator::m_instance->m_controlInterface->simulationData.digitalModule[0] :
		Simulator::m_instance->m_controlInterface->simulationData.digitalModule[1]);

	assert(!mod.io[channel-1].used && "io channel already used");		
	mod.io[channel-1].used = true;
}

void Simulator::FillDigitalIoSlot(UINT32 slot, UINT32 channel)
{	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		Simulator::m_instance->m_controlInterface->simulationData.digitalModule[0] :
		Simulator::m_instance->m_controlInterface->simulationData.digitalModule[1]);

	assert(!mod.io[channel-1].used && "io channel already used");		
	mod.io[channel-1].used = true;
}



/********************************************************************
 * Annoying copy/paste routines, but necessary evil
 ********************************************************************/
 
void Simulator::AddEncoder(Encoder * e, UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
	
	{
		wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
		
		Simulator::FillDigitalSlot(aSlot, aChannel);
		Simulator::FillDigitalSlot(bSlot, bChannel);
		
		Simulator::m_instance->m_controlInterface->simulationData.encoders.push_back(EncoderInfo(e, aSlot, aChannel, bSlot, _bChannel));
	}
}

void Simulator::DeleteEncoder(Encoder * e)
{
	if (!Simulator::m_instance)
		return;
		
	wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
		
	vector<EncoderInfo> * encoders = &Simulator::m_instance->m_controlInterface->simulationData.m_encoders;
	
	for (size_t i = 0; i < encoders->size(); i++)
		if ((*encoders)[i].encoder == e)
		{
			encoders->erase(encoders->begin() + i);
			break;
		}
}

void Simulator::AddGyro(Gyro * g, UINT32 slot, UINT32 channel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
		
	Simulator::m_instance->m_gyros.push_back(GyroInfo(g, slot, channel));
}

void Simulator::DeleteGyro(Gyro * g)
{
	if (!Simulator::m_instance)
		return;
		
	vector<GyroInfo> * gyros = &Simulator::m_instance->m_gyros;
	
	for (size_t i = 0; i < gyros->size(); i++)
		if ((*gyros)[i].gyro == g)
		{
			gyros->erase(gyros->begin() + i);
			break;
		}
}



void Simulator::AddPWM(PWM * p, UINT32 slot, UINT32 channel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
	
	{
		wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
		
		assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
		assert(channel > 0 && channel <= DIGITAL_PWM_CHANNELS);
		
		DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
			Simulator::m_instance->m_controlInterface->simulationData.digitalModule[0] :
			Simulator::m_instance->m_controlInterface->simulationData.digitalModule[1]);

		assert(!mod.pwm[channel-1].pwm && "pwm already used");
			
		mod.pwm[channel-1].pwm = p;
	}
}

void Simulator::DeletePWM(PWM * p)
{
	if (!Simulator::m_instance)
		return;
		
	{
		wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
		
		DigitalModuleData &mod = Simulator::m_instance->m_controlInterface->simulationData.digitalModule[0];
	
		for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
			if (mod.pwm[i].pwm == p)
			{
				mod.pwm[i].pwm = NULL;
				mod.pwm[i].speed = 0;
				return;
			}
			
		mod = Simulator::m_instance->m_controlInterface->simulationData.digitalModule[1];
			
		for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
			if (mod.pwm[i].pwm == p)
			{
				mod.pwm[i].pwm = NULL;
				mod.pwm[i].speed = 0;
				return;
			}
	}
}



void Simulator::AddDigitalInput(DigitalInput * di, UINT32 slot, UINT32 channel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
	
	{
		wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
		
		assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
		assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
		DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
			Simulator::m_instance->m_controlInterface->simulationData.digitalModule[0] :
			Simulator::m_instance->m_controlInterface->simulationData.digitalModule[1]);

		assert(!mod.io[channel].digitalInput && 
				!mod.io[channel].digitalOutput && "io already used");
			
		mod.io[channel].io = p;
	}
}

void Simulator::DeleteDigitalInput(DigitalInput * di)
{
	if (!Simulator::m_instance)
		return;
		
	{
		wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
		
		DigitalModuleData &mod = Simulator::m_instance->m_controlInterface->simulationData.digitalModule[0];
	
		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
			if (mod.io[i].io == p)
			{
				mod.io[i].io = NULL;
				mod.io[i].speed = 0;
				return;
			}
			
		mod = Simulator::m_instance->m_controlInterface->simulationData.digitalModule[1];
			
		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
			if (mod.io[i].io == p)
			{
				mod.io[i].io = NULL;
				mod.io[i].speed = 0;
				return;
			}
	}
}



void Simulator::AddNotifier(Notifier * n)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
		
	Simulator::m_instance->m_notifiers.push_back(n);
}

void Simulator::DeleteNotifier(Notifier * n)
{
	if (!Simulator::m_instance)
		return;
	
	vector<Notifier *> * notifiers = &Simulator::m_instance->m_notifiers;
	
	for (size_t i = 0; i < notifiers->size(); i++)
		if ((*notifiers)[i] == n)
		{
			notifiers->erase(notifiers->begin() + i);
			break;
		}
}

	

	
