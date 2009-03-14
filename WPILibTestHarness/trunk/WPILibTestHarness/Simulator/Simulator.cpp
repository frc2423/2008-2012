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

#include <vector>
using namespace std;

#ifdef USE_MSVC_MEMORY_TRACING
	// this enables advanced memory leak detection
	#include <wx/msw/msvcrt.h>
#endif


Simulator * Simulator::m_instance = NULL;

Simulator::Simulator(ControlInterface * controlInterface) :
	m_time(0),
	m_controlInterface(controlInterface)
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

		PacketReady();
	
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
	wxMutexLocker mtx(m_controlInterface->lock);

	if (userDsLcdDataLength > sizeof(m_controlInterface->simulationData.lcdText))
		userDsLcdDataLength = sizeof(m_controlInterface->simulationData.lcdText);
		
	memcpy(m_controlInterface->simulationData.lcdText, userDsLcdData, userDsLcdDataLength);
}
	
// retrieves control information
int Simulator::GetControlData(FRCControlData *data, char *userData)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	memcpy(data, &m_controlInterface->controlData, sizeof(FRCControlData));
	return 0;
}


void Simulator::FillDigitalIoSlot(UINT32 slot, UINT32 channel)
{	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		m_controlInterface->simulationData.digitalModule[0] :
		m_controlInterface->simulationData.digitalModule[1]);

	assert(!mod.io[channel-1].used && "io channel already used");		
	mod.io[channel-1].used = true;
}

void Simulator::FreeDigitalIoSlot(UINT32 slot, UINT32 channel)
{	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		m_controlInterface->simulationData.digitalModule[0] :
		m_controlInterface->simulationData.digitalModule[1]);

	assert(mod.io[channel-1].used && "io channel not used");		
	mod.io[channel-1].used = false;
}

void Simulator::FillAnalogIoSlot(UINT32 slot, UINT32 channel)
{	
	assert(slot == ANALOG_SLOT_1 || slot == ANALOG_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
	AnalogModuleData &mod = (slot == ANALOG_SLOT_1 ? 
		m_controlInterface->simulationData.analogModule[0] :
		m_controlInterface->simulationData.analogModule[1]);

	assert(!mod.io[channel-1].used && "io channel already used");		
	mod.io[channel-1].used = true;
}

void Simulator::FreeAnalogIoSlot(UINT32 slot, UINT32 channel)
{	
	assert(slot == ANALOG_SLOT_1 || slot == ANALOG_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
		
	AnalogModuleData &mod = (slot == ANALOG_SLOT_1 ? 
		m_controlInterface->simulationData.analogModule[0] :
		m_controlInterface->simulationData.analogModule[1]);

	assert(mod.io[channel-1].used && "io channel not used");		
	mod.io[channel-1].used = false;
}



/********************************************************************
 * Annoying copy/paste routines, but necessary evil. Ugly crap. 
 ********************************************************************/
 
void Simulator::AddEncoder(Encoder * e, UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 bChannel)
{
	wxMutexLocker mtx(m_controlInterface->lock);
		
	FillDigitalIoSlot(aSlot, aChannel);
	FillDigitalIoSlot(bSlot, bChannel);
		
	m_controlInterface->simulationData.encoders.push_back(EncoderInfo(e, aSlot, aChannel, bSlot, bChannel));
}

void Simulator::DeleteEncoder(Encoder * e)
{
	wxMutexLocker mtx(m_controlInterface->lock);
		
	vector<EncoderInfo> * encoders = &m_controlInterface->simulationData.encoders;
	
	for (size_t i = 0; i < encoders->size(); i++)
		if ((*encoders)[i].encoder == e)
		{
			FreeDigitalIoSlot((*encoders)[i].slota, (*encoders)[i].channela);
			FreeDigitalIoSlot((*encoders)[i].slotb, (*encoders)[i].channelb);
			encoders->erase(encoders->begin() + i);
			break;
		}
}


void Simulator::AddGyro(Gyro * g, UINT32 slot, UINT32 channel)
{
	wxMutexLocker mtx(m_controlInterface->lock);
		
	m_controlInterface->simulationData.gyros.push_back(GyroInfo(g, slot, channel));
	FillAnalogIoSlot(slot, channel);
}

void Simulator::DeleteGyro(Gyro * g)
{
	wxMutexLocker mtx(m_controlInterface->lock);
		
	vector<GyroInfo> * gyros = &m_controlInterface->simulationData.gyros;
	
	for (size_t i = 0; i < gyros->size(); i++)
		if ((*gyros)[i].gyro == g)
		{
			FreeAnalogIoSlot((*gyros)[i].slot, (*gyros)[i].channel);
			gyros->erase(gyros->begin() + i);
			break;
		}
}



void Simulator::AddPWM(PWM * p, UINT32 slot, UINT32 channel)
{

	wxMutexLocker mtx(m_controlInterface->lock);
	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_PWM_CHANNELS);
	
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		m_controlInterface->simulationData.digitalModule[0] :
		m_controlInterface->simulationData.digitalModule[1]);

	assert(!mod.pwm[channel-1].pwm && "pwm already used");
		
	mod.pwm[channel-1].pwm = p;
}

void Simulator::DeletePWM(PWM * p)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	
	{
		DigitalModuleData &mod = m_controlInterface->simulationData.digitalModule[0];

		for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
			if (mod.pwm[i].pwm == p)
			{
				mod.pwm[i].pwm = NULL;
				mod.pwm[i].speed = 0;
				return;
			}
	}
		
	{
		DigitalModuleData &mod = m_controlInterface->simulationData.digitalModule[1];

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
	wxMutexLocker mtx(m_controlInterface->lock);
	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
	
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		m_controlInterface->simulationData.digitalModule[0] :
		m_controlInterface->simulationData.digitalModule[1]);

	assert(
		!mod.io[channel-1].used &&
		!mod.io[channel-1].digitalInput && 
		!mod.io[channel-1].digitalOutput && "io already used");
		
	mod.io[channel-1].digitalInput = di;
	mod.io[channel-1].used = true;
}

void Simulator::DeleteDigitalInput(DigitalInput * di)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	
	{
		DigitalModuleData &mod = m_controlInterface->simulationData.digitalModule[0];

		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
			if (mod.io[i].digitalInput == di)
			{
				mod.io[i].digitalInput = NULL;
				mod.io[i].used = false;
				return;
			}
	}
		
	{
		DigitalModuleData &mod = m_controlInterface->simulationData.digitalModule[1];

		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
			if (mod.io[i].digitalInput == di)
			{
				mod.io[i].digitalInput = NULL;
				mod.io[i].used = false;
				return;
			}
	}
}

void Simulator::AddDigitalOutput(DigitalOutput * di, UINT32 slot, UINT32 channel)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	
	assert(slot == DIGITAL_SLOT_1 || slot == DIGITAL_SLOT_2);
	assert(channel > 0 && channel <= DIGITAL_IO_CHANNELS);
	
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		m_controlInterface->simulationData.digitalModule[0] :
		m_controlInterface->simulationData.digitalModule[1]);

	assert(
		!mod.io[channel-1].used &&
		!mod.io[channel-1].digitalOutput && 
		!mod.io[channel-1].digitalOutput && "io already used");
		
	mod.io[channel-1].digitalOutput = di;
	mod.io[channel-1].used = true;
}

void Simulator::DeleteDigitalOutput(DigitalOutput * di)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	
	{
		DigitalModuleData &mod = m_controlInterface->simulationData.digitalModule[0];

		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
			if (mod.io[i].digitalOutput == di)
			{
				mod.io[i].digitalOutput = NULL;
				mod.io[i].used = false;
				return;
			}
	}
		
	{
		DigitalModuleData &mod = m_controlInterface->simulationData.digitalModule[1];

		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
			if (mod.io[i].digitalOutput == di)
			{
				mod.io[i].digitalOutput = NULL;
				mod.io[i].used = false;
				return;
			}
	}
}


void Simulator::AddAnalogChannel(AnalogChannel * ac, UINT32 slot, UINT32 channel)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	
	assert(slot == ANALOG_SLOT_1 || slot == ANALOG_SLOT_2);
	assert(channel > 0 && channel <= ANALOG_IO_CHANNELS);
	
	AnalogModuleData &mod = (slot == ANALOG_SLOT_1 ? 
		m_controlInterface->simulationData.analogModule[0] :
		m_controlInterface->simulationData.analogModule[1]);

	assert(
		!mod.io[channel-1].used &&
		!mod.io[channel-1].analogChannel && "io already used");
		
	mod.io[channel-1].analogChannel = ac;
	mod.io[channel-1].used = true;
}

void Simulator::DeleteAnalogChannel(AnalogChannel * ac)
{
	wxMutexLocker mtx(m_controlInterface->lock);
	
	{
		AnalogModuleData &mod = m_controlInterface->simulationData.analogModule[0];

		for (size_t i = 0; i < ANALOG_IO_CHANNELS; i++)
			if (mod.io[i].analogChannel == ac)
			{
				mod.io[i].analogChannel = NULL;
				mod.io[i].used = false;
				return;
			}
	}
		
	{
		AnalogModuleData &mod = m_controlInterface->simulationData.analogModule[1];

		for (size_t i = 0; i < ANALOG_IO_CHANNELS; i++)
			if (mod.io[i].analogChannel == ac)
			{
				mod.io[i].analogChannel = NULL;
				mod.io[i].used = false;
				return;
			}
	}
}

void Simulator::AddSolenoid(Solenoid * p, UINT32 slot, UINT32 channel)
{

	wxMutexLocker mtx(m_controlInterface->lock);
	
	assert(slot == SOLENOID_SLOT_1);
	assert(channel > 0 && channel <= SOLENOID_IO_CHANNELS);
	
	DigitalModuleData &mod = (slot == DIGITAL_SLOT_1 ? 
		m_controlInterface->simulationData.digitalModule[0] :
		m_controlInterface->simulationData.digitalModule[1]);

	assert(!m_controlInterface->simulationData.solenoidModule.solenoids[channel-1].solenoid && "Solenoid already used");
		
	m_controlInterface->simulationData.solenoidModule.solenoids[channel-1].solenoid = p;
}

void Simulator::DeleteSolenoid(Solenoid * p)
{
	wxMutexLocker mtx(m_controlInterface->lock);

	SolenoidModule &mod = m_controlInterface->simulationData.solenoidModule;

	for (size_t i = 0; i < SOLENOID_IO_CHANNELS; i++)
		if (mod.solenoids[i].solenoid == p)
		{
			mod.solenoids[i].solenoid = NULL;
			mod.solenoids[i].value = false;
			return;
		}
}



void Simulator::AddNotifier(Notifier * n)
{
	m_notifiers.push_back(n);
}

void Simulator::DeleteNotifier(Notifier * n)
{	
	for (size_t i = 0; i < m_notifiers.size(); i++)
		if (m_notifiers[i] == n)
		{
			m_notifiers.erase(m_notifiers.begin() + i);
			break;
		}
}

	

	
