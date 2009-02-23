

#ifndef SIMULATION_WINDOW_H
#define SIMULATION_WINDOW_H

#include <wx/spinctrl.h>
#include "Simulator/ControlInterface.h"


class SimulationWindow : public wxFrame, public wxThreadHelper
{
public:
	SimulationWindow(wxWindow * parent);

private:

	SimulationWindow();
	
	// entry point for thread
	wxThread::ExitCode Entry();
	
	void BeginSimulation();
	void KillSimulation();
	
	void OnClose(wxCloseEvent &event);
	void OnStartClicked(wxCommandEvent &event);
	void OnStepClicked(wxCommandEvent &event);
	
	signed char GetJoystickValue(wxTextCtrl * ctrl);
	
	void OnSimulationTimer(wxTimerEvent &event);
	void OnStep(wxCommandEvent &event);

	// xrc elements
	wxTextCtrl *			m_joy1X;
	wxTextCtrl *			m_joy1Y;
	wxTextCtrl *			m_joy1T;
	
	wxTextCtrl *			m_joy2X;
	wxTextCtrl *			m_joy2Y;
	wxTextCtrl *			m_joy2T;
	
	wxCheckBox *			m_calibrateBox;
	wxSpinCtrl *			m_mode;
	
	wxCheckBox * 			m_enabledBox;
	wxCheckBox *			m_autonomousBox;
	
	wxButton *				m_startButton;
	wxTextCtrl *			m_stepText;
	wxTextCtrl * 			m_timeText;

	wxStatusBar *			m_statusBar;
	
	ControlInterface		m_controlInterface;

	// simulation thread variables
	wxMutex					m_threadMutex;
	wxCondition				m_threadCondition;
	
	bool 					m_started;
	size_t					m_stepsLeft;
	
	wxTimer					m_simulationTimer;
	
	DECLARE_EVENT_TABLE()
};




#endif