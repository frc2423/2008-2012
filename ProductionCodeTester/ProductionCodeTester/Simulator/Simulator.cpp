
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
{
	memset(m_lcdText, ' ', sizeof(m_lcdText));
}

// does not return until simulation is complete
void Simulator::StartSimulation(ControlInterface * controlInterface)
{
	assert(!Simulator::m_instance);
	
	// create the simulator
	Simulator::m_instance = new Simulator(controlInterface);
	StartRobotClass();
	
	// route the hardware to the right places
	Simulator::m_instance->RouteHardware();
	
	// begin the simulation by calling into the user's code
	RobotBase::startRobotTask(NULL);
	
	// and then delete self when done
	delete Simulator::m_instance;
	Simulator::m_instance = NULL;
}

void Simulator::RouteHardware()
{

}


void Simulator::SimulateStep(double tm)
{
	{ 
		// perform calculations while protected by the lock
		wxMutexLocker mtx(m_controlInterface->lock);

		
		// first, calculate the next step for motors and such
		//for (size_t i = 0; i < m_wheels.size(); i++)
		//	m_wheels[i].NextStep();

		// next, calculate the overall position of the robot
		
		
		// if in single-step mode, then wait for the signal
		// to step
	
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

	// returning from this function gives control to the control loop
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

	if (userDsLcdDataLength > sizeof(Simulator::m_instance->m_lcdText))
		userDsLcdDataLength = sizeof(Simulator::m_instance->m_lcdText);
	memcpy(Simulator::m_instance->m_lcdText, userDsLcdData, userDsLcdDataLength);
}
	
// retrieves control information
int Simulator::GetControlData(FRCControlData *data, char *userData)
{
	wxMutexLocker mtx(Simulator::m_instance->m_controlInterface->lock);
	memcpy(data, &Simulator::m_instance->m_controlInterface->controlData, sizeof(FRCControlData));
	return 0;
}

/********************************************************************
 * Annoying copy/paste routines, but necessary evil
 ********************************************************************/

void Simulator::AddEncoder(Encoder * e, UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;

	Simulator::m_instance->m_encoders.push_back(EncoderInfo(e, aSlot, aChannel, bSlot, _bChannel));
}

void Simulator::DeleteEncoder(Encoder * e)
{
	if (!Simulator::m_instance)
		return;
	
	vector<EncoderInfo> * encoders = &Simulator::m_instance->m_encoders;
	
	for (size_t i = 0; i < encoders->size(); i++)
		if ((*encoders)[i].encoder == e)
		{
			encoders->erase(encoders->begin() + i);
			break;
		}
}

void Simulator::AddPWM(PWM * p, UINT32 slot, UINT32 channel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
		
	Simulator::m_instance->m_pwms.push_back(PWMInfo(p, slot, channel));
}

void Simulator::DeletePWM(PWM * p)
{
	if (!Simulator::m_instance)
		return;
	
	vector<PWMInfo> * pwms = &Simulator::m_instance->m_pwms;
	
	for (size_t i = 0; i < pwms->size(); i++)
		if ((*pwms)[i].pwm == p)
		{
			pwms->erase(pwms->begin() + i);
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

void Simulator::AddDigitalInput(DigitalInput * di, UINT32 slot, UINT32 channel)
{
	assert(Simulator::m_instance);
	if (!Simulator::m_instance)
		return;
		
	Simulator::m_instance->m_digitalInputs.push_back(DigitalInputInfo(di, slot, channel));
}

void Simulator::DeleteDigitalInput(DigitalInput * di)
{
	if (!Simulator::m_instance)
		return;
		
	vector<DigitalInputInfo> * digitalInputs = &Simulator::m_instance->m_digitalInputs;
	
	for (size_t i = 0; i < digitalInputs->size(); i++)
		if ((*digitalInputs)[i].digitalInput == di)
		{
			digitalInputs->erase(digitalInputs->begin() + i);
			break;
		}
}
	

	
