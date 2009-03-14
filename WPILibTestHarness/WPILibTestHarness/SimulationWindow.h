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
#include "Slider.h"
#include "TogglePanelButton.h"


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
	
	void OnSimulationTimer(wxTimerEvent &event);
	void OnStep(wxCommandEvent &event);

	void OnDrawTimer(wxTimerEvent &event);

	void OnJoystick(wxJoystickEvent &event);
	double ConvertJoystickByte(int value, int min, int max);
	signed char GetJoystickValue(double val);

	wxJoystick				m_joystick1;

	// UI elements
	Slider *				m_joy1_x;
	Slider *				m_joy1_y;
	Slider *				m_joy1_z;
	Slider *				m_joy1_t;

	wxCheckBox *			m_joy1_1;
	wxCheckBox *			m_joy1_2;
	wxCheckBox *			m_joy1_3;
	wxCheckBox *			m_joy1_4;

	wxCheckBox *			m_joy1_enable;

	
	Slider *				m_joy2_x;
	Slider *				m_joy2_y;
	Slider *				m_joy2_z;
	Slider *				m_joy2_t;

	wxCheckBox *			m_joy2_1;
	wxCheckBox *			m_joy2_2;
	wxCheckBox *			m_joy2_3;
	wxCheckBox *			m_joy2_4;
	
	// driver station inputs
	wxCheckBox *			m_ds_i_1;
	wxCheckBox *			m_ds_i_2;
	wxCheckBox *			m_ds_i_3;
	wxCheckBox *			m_ds_i_4;
	wxCheckBox *			m_ds_i_5;
	wxCheckBox *			m_ds_i_6;
	wxCheckBox *			m_ds_i_7;
	wxCheckBox *			m_ds_i_8;

	Slider *				m_ds_ai_1;
	Slider *				m_ds_ai_2;
	Slider *				m_ds_ai_3;
	Slider *				m_ds_ai_4;
	
	// control inputs
	wxCheckBox * 			m_enabledBox;
	wxCheckBox *			m_autonomousBox;
	
	wxButton *				m_startButton;
	wxTextCtrl *			m_stepText;

	wxStatusBar *			m_statusBar;

	wxTextCtrl *			m_lcdTop;
	wxTextCtrl *			m_lcdBottom;

	// analog inputs
	Slider					* m_slot1Analog[ANALOG_IO_CHANNELS];
	Slider					* m_slot2Analog[ANALOG_IO_CHANNELS];

	// pwm displays
	Slider					* m_slot1PWM[DIGITAL_PWM_CHANNELS];
	Slider					* m_slot2PWM[DIGITAL_PWM_CHANNELS];

	// digital displays
	TogglePanelButton		* m_slot1DIO[DIGITAL_IO_CHANNELS];
	wxStaticText			* m_slot1DIO_lbl[DIGITAL_IO_CHANNELS];

	TogglePanelButton		* m_slot2DIO[DIGITAL_IO_CHANNELS];
	wxStaticText			* m_slot2DIO_lbl[DIGITAL_IO_CHANNELS];

	// simulation thread variables
	wxMutex					m_threadMutex;
	wxCondition				m_threadCondition;
	
	bool					m_data_ready;
	bool 					m_started;
	size_t					m_stepsLeft;

	ControlInterface		m_controlInterface;
	
	wxTimer					m_simulationTimer;
	wxTimer					m_drawTimer;
	
	DECLARE_EVENT_TABLE()
};




#endif