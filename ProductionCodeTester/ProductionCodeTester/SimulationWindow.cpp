

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
	EVT_BUTTON( XRCID("m_startButton"), SimulationWindow::OnStartClicked )
	EVT_BUTTON( XRCID("m_stepButton"), SimulationWindow::OnStepClicked )
	EVT_COMMAND( 42, EVT_ON_STEP, SimulationWindow::OnStep )
	EVT_TIMER( 1, SimulationWindow::OnSimulationTimer)
END_EVENT_TABLE()


#define XRC_INIT(name, type)	name = XRCCTRL(*this, #name, type)

SimulationWindow::SimulationWindow(wxWindow *parent) :
	m_threadCondition(m_threadMutex),
	m_controlInterface(this),
	m_started(false),
	m_simulationTimer(this, 1)
{
	// initialize XRC elements..
	wxXmlResource::Get()->LoadFrame(this, parent, wxT("SimulationWindow"));

	XRC_INIT(m_joy1X, wxTextCtrl);
	XRC_INIT(m_joy1Y, wxTextCtrl);
	XRC_INIT(m_joy1T, wxTextCtrl);
	
	XRC_INIT(m_joy2X, wxTextCtrl);
	XRC_INIT(m_joy2Y, wxTextCtrl);
	XRC_INIT(m_joy2T, wxTextCtrl);
	
	XRC_INIT(m_calibrateBox, wxCheckBox);
	XRC_INIT(m_mode, wxSpinCtrl);
	XRC_INIT(m_stepText, wxTextCtrl);
	XRC_INIT(m_timeText, wxTextCtrl);
	
	XRC_INIT(m_startButton, wxButton);
	XRC_INIT(m_statusBar, wxStatusBar);
	
	XRC_INIT(m_enabledBox, wxCheckBox);
	XRC_INIT(m_autonomousBox, wxCheckBox);
	
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
	
	// initialize the simulation thread
	BeginSimulation();
	
	// anything else that needs to be done
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
		
		data->stick0Axis1 = GetJoystickValue(m_joy1X);
		data->stick0Axis2 = GetJoystickValue(m_joy1Y);
		data->stick0Axis4 = GetJoystickValue(m_joy1T);
		data->stick2Axis1 = GetJoystickValue(m_joy2X);
		data->stick2Axis2 = GetJoystickValue(m_joy2Y);
		data->stick2Axis4 = GetJoystickValue(m_joy2T);
	}
	
	m_controlInterface.Step();
}
	
void SimulationWindow::OnStep(wxCommandEvent &event)
{
	m_statusBar->SetStatusText(wxString::Format(wxT("%.3f"), Simulator::GetTime()), 1);
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



