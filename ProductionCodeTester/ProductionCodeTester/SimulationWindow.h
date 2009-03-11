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

#ifndef SIMULATION_WINDOW_H
#define SIMULATION_WINDOW_H


#include <wx/joystick.h>
#include <wx/spinctrl.h>
#include "Simulator/ControlInterface.h"

#include "MotorDisplay.h"


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

	void OnEnableJoy1(wxCommandEvent &event);
	
	signed char GetJoystickValue(wxTextCtrl * ctrl);
	
	void OnSimulationTimer(wxTimerEvent &event);
	void OnStep(wxCommandEvent &event);

	void OnDrawTimer(wxTimerEvent &event);

	void OnNewMode(wxSpinEvent &event);

	wxJoystick				m_joystick1;

	// xrc elements
	wxCheckBox *			m_enableJoy1;

	wxTextCtrl *			m_joy1X;
	wxTextCtrl *			m_joy1Y;
	wxTextCtrl *			m_joy1T;
	
	wxTextCtrl *			m_joy2X;
	wxTextCtrl *			m_joy2Y;
	wxTextCtrl *			m_joy2T;
	
	wxSpinCtrl *			m_analogIn1;
	wxSpinCtrl *			m_analogIn2;
	wxSpinCtrl *			m_analogIn3;
	wxSpinCtrl *			m_analogIn4;
	
	wxCheckBox *			m_calibrateBox;
	wxSpinCtrl *			m_mode;
	
	wxCheckBox * 			m_enabledBox;
	wxCheckBox *			m_autonomousBox;
	
	wxButton *				m_startButton;
	wxTextCtrl *			m_stepText;
	wxTextCtrl * 			m_timeText;

	wxStatusBar *			m_statusBar;

	wxSizer *				m_motorsSizer;
	wxSizer *				m_displaySizer;

	wxTextCtrl *			m_lcdTop;
	wxTextCtrl *			m_lcdBottom;
	
	ControlInterface		m_controlInterface;

	MotorDisplay			*m_motor1;

	// simulation thread variables
	wxMutex					m_threadMutex;
	wxCondition				m_threadCondition;
	
	bool					m_data_ready;
	bool 					m_started;
	size_t					m_stepsLeft;
	
	wxTimer					m_simulationTimer;
	wxTimer					m_drawTimer;
	
	DECLARE_EVENT_TABLE()
};




#endif