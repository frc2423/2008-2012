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

#include <ctime>
#include <cstdlib>

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include <VxWorks/VxWorks.h>
#include <WPILib/NetworkCommunication/FRCComm.h>

#include "SimulationWindow.h"
#include "Simulator/Simulator.h"

#ifdef USE_MSVC_MEMORY_TRACING
	// this enables advanced memory leak detection
	#include <wx/msw/msvcrt.h>
#endif


DEFINE_EVENT_TYPE(EVT_ON_STEP)

BEGIN_EVENT_TABLE(SimulationWindow, wxFrame)
	EVT_CLOSE( SimulationWindow::OnClose )
	EVT_CHECKBOX( XRCID("m_enableJoy1"), SimulationWindow::OnEnableJoy1 )
	EVT_BUTTON( XRCID("m_startButton"), SimulationWindow::OnStartClicked )
	EVT_BUTTON( XRCID("m_stepButton"), SimulationWindow::OnStepClicked )
	EVT_SPINCTRL( XRCID("m_mode"), SimulationWindow::OnNewMode)
	EVT_COMMAND( 42, EVT_ON_STEP, SimulationWindow::OnStep )
	EVT_TIMER( 1, SimulationWindow::OnSimulationTimer)
	EVT_TIMER( 2, SimulationWindow::OnDrawTimer)
END_EVENT_TABLE()


#define XRC_INIT(name, type)	name = XRCCTRL(*this, #name, type)

SimulationWindow::SimulationWindow(wxWindow *parent) :
	m_threadCondition(m_threadMutex),
	m_controlInterface(this),
	m_started(false),
	m_simulationTimer(this, 1),
	m_drawTimer(this, 2)
{
	srand(time(NULL));

	// initialize XRC elements..
	wxXmlResource::Get()->LoadFrame(this, parent, wxT("SimulationWindow"));

	XRC_INIT(m_enableJoy1, wxCheckBox);

	XRC_INIT(m_joy1X, wxTextCtrl);
	XRC_INIT(m_joy1Y, wxTextCtrl);
	XRC_INIT(m_joy1T, wxTextCtrl);
	
	XRC_INIT(m_joy2X, wxTextCtrl);
	XRC_INIT(m_joy2Y, wxTextCtrl);
	XRC_INIT(m_joy2T, wxTextCtrl);
	
	XRC_INIT(m_analogIn1, wxSpinCtrl);
	XRC_INIT(m_analogIn2, wxSpinCtrl);
	XRC_INIT(m_analogIn3, wxSpinCtrl);
	XRC_INIT(m_analogIn4, wxSpinCtrl);
	
	XRC_INIT(m_calibrateBox, wxCheckBox);
	XRC_INIT(m_mode, wxSpinCtrl);
	XRC_INIT(m_stepText, wxTextCtrl);
	XRC_INIT(m_timeText, wxTextCtrl);
	
	XRC_INIT(m_startButton, wxButton);
	XRC_INIT(m_statusBar, wxStatusBar);
	
	XRC_INIT(m_enabledBox, wxCheckBox);
	XRC_INIT(m_autonomousBox, wxCheckBox);

	XRC_INIT(m_lcdTop, wxTextCtrl);
	XRC_INIT(m_lcdBottom, wxTextCtrl);

	wxStaticText * tmp;
	
	tmp = XRCCTRL(*this, "m_getMotorsSizer", wxStaticText);
	m_motorsSizer = tmp->GetContainingSizer();

	tmp = XRCCTRL(*this, "m_getDisplaySizer", wxStaticText);
	m_displaySizer = tmp->GetContainingSizer();
	
	// setup validation
	m_joy1X->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	m_joy1Y->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	m_joy1T->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	
	m_joy2X->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	m_joy2Y->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	m_joy2T->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	
	m_stepText->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	m_timeText->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	
	// statusbar
	m_statusBar->SetFieldsCount(2);
	m_statusBar->SetStatusText(wxT("Stopped"), 0);
	m_statusBar->SetStatusText(wxT("0.0s"), 1);

	wxPanel * p = XRCCTRL(*this, "m_topPanel", wxPanel);
	m_motor1 = new MotorDisplay(p);
	m_motorsSizer->Add( m_motor1 );

	//SetAutoLayout(true);

	//
	p->Layout();

	//Fit();
	//SetAutoLayout(true);
	//Layout();

	// initialize the simulation thread
	BeginSimulation();
	
	// anything else that needs to be done

	// try to update regularly
	m_drawTimer.Start(200);
}

void SimulationWindow::OnNewMode(wxSpinEvent &event)
{
	m_motor1->SetValue(((double)m_mode->GetValue() - 5.0)/5.0);
}

void SimulationWindow::OnClose(wxCloseEvent &event)
{
	KillSimulation();
	Destroy();
}

void SimulationWindow::OnStartClicked(wxCommandEvent &event)
{
	m_stepsLeft = 0;

	if (!m_started)
	{
		m_startButton->SetLabel(wxT("Stop"));
		m_statusBar->SetStatusText(wxT("Running"), 0);
		m_simulationTimer.Start(25);
	}
	else
	{
		m_startButton->SetLabel(wxT("Start"));
		m_statusBar->SetStatusText(wxT("Stopped"), 0);
		m_simulationTimer.Stop();
	}
		
	m_started = !m_started;
}

void SimulationWindow::OnStepClicked(wxCommandEvent &event)
{
	long l;
	if (!m_stepText->GetValue().ToLong(&l) || l < 1)
	{
		wxMessageBox(wxT("Invalid number of steps"), wxT("Error"));
		return;
	}
	
	m_statusBar->SetStatusText(wxT("Running"), 0);
	m_stepsLeft = l;
	
	m_simulationTimer.Start(25);
}

void SimulationWindow::OnEnableJoy1(wxCommandEvent &event)
{
	if (m_enableJoy1->IsChecked())
	{
		m_joy1X->Disable();
		m_joy1Y->Disable();
		m_joy1T->Disable();
	}
	else
	{
		m_joy1X->Enable();
		m_joy1Y->Enable();
		m_joy1T->Enable();
	}
}

static 
int AddNoise(int value)
{
	int ret = ((rand() % 10) - 5) + value;
	
	if (ret < 0)
		ret = 0;
	else if (ret > 1023)
		ret = 1023;
		
	return ret;
}
	
void SimulationWindow::OnSimulationTimer(wxTimerEvent &event)
{
	if (m_stepsLeft)
	{
		m_stepsLeft -= 1;
		if (!m_stepsLeft)
		{
			m_statusBar->SetStatusText(wxT("Stopped"), 0);
			m_simulationTimer.Stop();
		}
	}
	
	// send the joystick values and other things
	{
		wxMutexLocker mtx(m_controlInterface.lock);
		
		FRCControlData * data = &m_controlInterface.controlData;
		
		data->packetIndex++;
		data->enabled = m_enabledBox->IsChecked();
		data->autonomous = m_autonomousBox->IsChecked();
		
		if (m_enableJoy1->IsChecked())
		{
			wxPoint pt = m_joystick1.GetPosition();

			data->stick0Axis1 = pt.x;
			data->stick0Axis2 = pt.y;
			data->stick0Axis3 = m_joystick1.GetZPosition();

			data->stick0Buttons = m_joystick1.GetButtonState();
		}
		else
		{
			data->stick0Axis1 = GetJoystickValue(m_joy1X);
			data->stick0Axis2 = GetJoystickValue(m_joy1Y);
			data->stick0Axis4 = GetJoystickValue(m_joy1T);
		}

		data->stick1Axis1 = GetJoystickValue(m_joy2X);
		data->stick1Axis2 = GetJoystickValue(m_joy2Y);
		data->stick1Axis4 = GetJoystickValue(m_joy2T);

		data->dsDigitalIn = 0;

		// port 1
		if (m_calibrateBox->IsChecked())
			data->dsDigitalIn |= 0x10;

		int mode = m_mode->GetValue();

		// port 4 is lsb
		if (mode & 0x01)
			data->dsDigitalIn |= 0x08;
		// port 3
		if (mode & 0x02)
			data->dsDigitalIn |= 0x04;
		// port 2
		if (mode & 0x04)
			data->dsDigitalIn |= 0x02;
		// port 1
		if (mode & 0x08)
			data->dsDigitalIn |= 0x01;
			
		// analog inputs
		data->analog1 = AddNoise(m_analogIn1->GetValue());
		data->analog2 = AddNoise(m_analogIn2->GetValue());
		data->analog3 = AddNoise(m_analogIn3->GetValue());
		data->analog4 = AddNoise(m_analogIn4->GetValue());
	}
	
	m_controlInterface.Step();
}
	
void SimulationWindow::OnStep(wxCommandEvent &event)
{
	m_statusBar->SetStatusText(wxString::Format(wxT("%.3f"), Simulator::GetTime()), 1);
	m_data_ready = true;
}


void SimulationWindow::BeginSimulation()
{
	if (GetThread() && GetThread()->IsRunning())
		KillSimulation();
	
	wxThreadHelper::Create();
	GetThread()->Run();
}

void SimulationWindow::KillSimulation()
{
	// lock the thread condition mutex
	m_threadMutex.Lock();
		
	m_controlInterface.Kill();
	
	//  give it 3 seconds to close, otherwise kill the thread
	if (!GetThread() || !GetThread()->IsRunning())
		m_threadMutex.Unlock();
		
	else if (m_threadCondition.WaitTimeout(5000) == wxCOND_TIMEOUT)
	{
		wxMessageBox(wxT("The simulation thread is not exiting properly! Restart application for best results"), wxT("Timeout"));
		GetThread()->Kill();
	}
}

// entrypoint for simulation thread
wxThread::ExitCode SimulationWindow::Entry()
{
	Simulator::StartSimulation(&m_controlInterface);
	
	{
		// exit gracefully, signaling the parent window
		wxMutexLocker mtx(m_threadMutex);
		m_threadCondition.Broadcast();
	}
	
	return 0;
}


signed char SimulationWindow::GetJoystickValue(wxTextCtrl * ctrl)
{
	double x;
		
	if (!ctrl->GetValue().ToDouble(&x))
		return 0;

	if (x < 0)
		return (signed char)(x * 128.0);
		
	return (signed char)(x * 127.0);
}

void SimulationWindow::OnDrawTimer(wxTimerEvent &event)
{
	if (!m_data_ready)
		return;

	SimulationData newData;

	// transfer info over in a threadsafe way
	{
		wxMutexLocker mtx(m_controlInterface.lock);
		newData = m_controlInterface.simulationData;
	}

	// then do the drawing

	// lcd text
	m_lcdTop->SetValue(wxString::FromUTF8(newData.lcdText + 2, 21));

	wxString str = wxString::FromUTF8(newData.lcdText + 23, sizeof(newData.lcdText)-23);
	
	for (size_t i = 1;i < 5; i++)
	{
		if (i*21+i-1 >= str.size())
			break;

		str.insert(i*21+i-1, wxT("\n"));
	}

	m_lcdBottom->SetValue(str);

	m_data_ready = false;
}

