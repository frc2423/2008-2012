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
	EVT_CHECKBOX( XRCID("m_joy1_enable"), SimulationWindow::OnEnableJoy1 )
	EVT_BUTTON( XRCID("m_startButton"), SimulationWindow::OnStartClicked )
	EVT_BUTTON( XRCID("m_stepButton"), SimulationWindow::OnStepClicked )
	EVT_COMMAND( 42, EVT_ON_STEP, SimulationWindow::OnStep )
	EVT_TIMER( 1, SimulationWindow::OnSimulationTimer)
	EVT_TIMER( 2, SimulationWindow::OnDrawTimer)

	EVT_JOYSTICK_EVENTS(SimulationWindow::OnJoystick)
END_EVENT_TABLE()


#define XRC_INIT(name, type)	name = XRCCTRL(*this, #name, type)

#define INIT_JOY(name)										\
	m_joy##name = new Slider(p, wxID_ANY, -1, 1, false);	\
	wxXmlResource::Get()->AttachUnknownControl(				\
			wxT("m_joy") wxT(#name),								\
			m_joy##name , p);

#define INIT_DS_ANALOG(name)										\
	m_ds_ai_##name = new Slider(p, wxID_ANY, 0, 5, false);	\
	wxXmlResource::Get()->AttachUnknownControl(				\
	wxT("m_ds_ai_") wxT(#name),								\
			m_ds_ai_##name , p);


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

	wxPanel * p = XRCCTRL(*this, "m_topPanel", wxPanel);

	XRC_INIT(m_joy1_enable, wxCheckBox);

	INIT_JOY(1_x)
	INIT_JOY(1_y)
	INIT_JOY(1_z)
	INIT_JOY(1_t)
	
	m_joy1_x->SetReadOnly(true);
	m_joy1_y->SetReadOnly(true);
	m_joy1_z->SetReadOnly(true);
	m_joy1_t->SetReadOnly(true);

	XRC_INIT(m_joy1_1, wxCheckBox);
	XRC_INIT(m_joy1_2, wxCheckBox);
	XRC_INIT(m_joy1_3, wxCheckBox);
	XRC_INIT(m_joy1_4, wxCheckBox);

	INIT_JOY(2_x)
	INIT_JOY(2_y)
	INIT_JOY(2_z)
	INIT_JOY(2_t)

	XRC_INIT(m_joy2_1, wxCheckBox);
	XRC_INIT(m_joy2_2, wxCheckBox);
	XRC_INIT(m_joy2_3, wxCheckBox);
	XRC_INIT(m_joy2_4, wxCheckBox);

	XRC_INIT(m_ds_i_1, wxCheckBox);
	XRC_INIT(m_ds_i_2, wxCheckBox);
	XRC_INIT(m_ds_i_3, wxCheckBox);
	XRC_INIT(m_ds_i_4, wxCheckBox);
	XRC_INIT(m_ds_i_5, wxCheckBox);
	XRC_INIT(m_ds_i_6, wxCheckBox);
	XRC_INIT(m_ds_i_7, wxCheckBox);
	XRC_INIT(m_ds_i_8, wxCheckBox);

	INIT_DS_ANALOG(1)
	INIT_DS_ANALOG(2)
	INIT_DS_ANALOG(3)
	INIT_DS_ANALOG(4)

	XRC_INIT(m_stepText, wxTextCtrl);
	
	XRC_INIT(m_startButton, wxButton);
	XRC_INIT(m_statusBar, wxStatusBar);
	
	XRC_INIT(m_enabledBox, wxCheckBox);
	XRC_INIT(m_autonomousBox, wxCheckBox);

	XRC_INIT(m_lcdTop, wxTextCtrl);
	XRC_INIT(m_lcdBottom, wxTextCtrl);
	
	// setup validation
	m_stepText->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	
	// statusbar
	m_statusBar->SetFieldsCount(2);
	m_statusBar->SetStatusText(wxT("Stopped"), 0);
	m_statusBar->SetStatusText(wxT("0.0s"), 1);

	// initialize analog inputs
	wxXmlResource * xmlRes = wxXmlResource::Get();

	// slot 1
	for (size_t i = 0; i < ANALOG_IO_CHANNELS; i++)
	{
		m_slot1Analog[i] = new Slider(p, wxID_ANY, 0, 5, false);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_analog_1_%d"), i+1), 
			m_slot1Analog[i], p);
	}

	// slot 2
	for (size_t i = 0; i < ANALOG_IO_CHANNELS; i++)
	{
		m_slot2Analog[i] = new Slider(p, wxID_ANY, 0, 5, false);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_analog_2_%d"), i+1), 
			m_slot2Analog[i], p);
	}


	// initialize pwm outputs

	// slot 4
	for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
	{
		m_slot1PWM[i] = new Slider(p, wxID_ANY, -1, 1, true);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_pwm_1_%d"), i+1), 
			m_slot1PWM[i], p);
	}

	// slot 6
	for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
	{
		m_slot2PWM[i] = new Slider(p, wxID_ANY, -1, 1, true);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_pwm_2_%d"), i+1), 
			m_slot2PWM[i], p);
	}
	
	// slot 8 (solenoids)
	for (size_t i = 0; i < SOLENOID_IO_CHANNELS; i++)
	{
		m_solenoids[i] = new TogglePanelButton(p, wxID_ANY);
		m_solenoids[i]->SetReadOnly(true);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_sol_%d"), i+1), 
			m_solenoids[i], p);
	}
	

	// initialize digital channels

	// slot 4
	for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
	{
		m_slot1DIO[i] = new TogglePanelButton(p, wxID_ANY);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_dio_1_%d"), i+1), 
			m_slot1DIO[i], p);

		m_slot1DIO_lbl[i] = wxStaticCast(FindWindow(xmlRes->GetXRCID(wxString::Format(wxT("m_diol_1_%d"), i+1))), 
			wxStaticText);
	}

	// slot 6
	for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
	{
		m_slot2DIO[i] = new TogglePanelButton(p, wxID_ANY);
		xmlRes->AttachUnknownControl(
			wxString::Format(wxT("m_dio_2_%d"), i+1), 
			m_slot2DIO[i], p);

		m_slot2DIO_lbl[i] = wxStaticCast(FindWindow(xmlRes->GetXRCID(wxString::Format(wxT("m_diol_2_%d"), i+1))), 
			wxStaticText);
	}


	p->Fit();
	p->Layout();

	Layout();

	// initialize the simulation thread
	BeginSimulation();
	
	// anything else that needs to be done

	m_joystick1.SetCapture(this);

	// try to update regularly
	m_drawTimer.Start(150);
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
		m_simulationTimer.Start(SIMULATOR_STEP_TIMER);
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
	
	m_simulationTimer.Start(SIMULATOR_STEP_TIMER);
}

void SimulationWindow::OnEnableJoy1(wxCommandEvent &event)
{
	bool enable = m_joy1_enable->IsChecked();

	m_joy1_x->SetReadOnly( enable );
	m_joy1_y->SetReadOnly( enable );
	m_joy1_z->SetReadOnly( enable );
	m_joy1_t->SetReadOnly( enable );

	m_joy1_1->Enable( !enable );
	m_joy1_2->Enable( !enable );
	m_joy1_3->Enable( !enable );
	m_joy1_4->Enable( !enable );

	if (enable)
		m_joystick1.SetCapture(this);

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
		
		// joystick values
		data->stick0Axis1 = GetJoystickValue(m_joy1_x->GetValue());
		data->stick0Axis2 = GetJoystickValue(m_joy1_y->GetValue());
		data->stick0Axis3 = GetJoystickValue(m_joy1_z->GetValue());
		data->stick0Axis4 = GetJoystickValue(m_joy1_t->GetValue());

		if (m_joy1_enable->IsChecked())
			data->stick0Buttons = m_joystick1.GetButtonState();
		else
			data->stick0Buttons = 
				((int) m_joy1_4->IsChecked() << 4) |
				((int) m_joy1_3->IsChecked() << 3) |
				((int) m_joy1_2->IsChecked() << 2) |
				(int) m_joy1_1->IsChecked();				

		data->stick1Axis1 = GetJoystickValue(m_joy2_x->GetValue());
		data->stick1Axis2 = GetJoystickValue(m_joy2_y->GetValue());
		data->stick1Axis3 = GetJoystickValue(m_joy2_z->GetValue());
		data->stick1Axis4 = GetJoystickValue(m_joy2_t->GetValue());

		data->stick1Buttons = 
				((int) m_joy2_4->IsChecked() << 4) |
				((int) m_joy2_3->IsChecked() << 3) |
				((int) m_joy2_2->IsChecked() << 2) |
				(int) m_joy2_1->IsChecked();

		data->dsDigitalIn = 0;

		// driver station I/O
		if (m_ds_i_1->IsChecked())
			data->dsDigitalIn |= 0x01;

		if (m_ds_i_2->IsChecked())
			data->dsDigitalIn |= 0x02;

		if (m_ds_i_3->IsChecked())
			data->dsDigitalIn |= 0x04;

		if (m_ds_i_4->IsChecked())
			data->dsDigitalIn |= 0x08;

		if (m_ds_i_5->IsChecked())
			data->dsDigitalIn |= 0x10;

		if (m_ds_i_6->IsChecked())
			data->dsDigitalIn |= 0x20;

		if (m_ds_i_7->IsChecked())
			data->dsDigitalIn |= 0x40;

		if (m_ds_i_8->IsChecked())
			data->dsDigitalIn |= 0x80;

			
		// analog inputs
		data->analog1 = (m_ds_ai_1->GetValue() * 1024) / 5.0;
		data->analog2 = (m_ds_ai_2->GetValue() * 1024) / 5.0;
		data->analog3 = (m_ds_ai_3->GetValue() * 1024) / 5.0;
		data->analog4 = (m_ds_ai_4->GetValue() * 1024) / 5.0;

		// slot 1
		{
			AnalogModuleData &mod = m_controlInterface.simulationData.analogModule[0];
		
			for (size_t i = 0; i < ANALOG_IO_CHANNELS; i++)
			{
				m_slot1Analog[i]->SetEnabled( mod.io[i].used );
				mod.io[i].value = m_slot1Analog[i]->GetValue();
			}
		}

		// slot 2
		{
			AnalogModuleData &mod = m_controlInterface.simulationData.analogModule[1];
		
			for (size_t i = 0; i < ANALOG_IO_CHANNELS; i++)
			{
				m_slot2Analog[i]->SetEnabled( mod.io[i].used );
				mod.io[i].value = m_slot2Analog[i]->GetValue();
			}
		}

		// slot 4
		{
			DigitalModuleData &mod = m_controlInterface.simulationData.digitalModule[0];
		
			// digital io
			for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
				if (mod.io[i].digitalInput)
					mod.io[i].value = m_slot1DIO[i]->GetValue();
		}
		
		// slot 6
		{
			DigitalModuleData &mod = m_controlInterface.simulationData.digitalModule[1];
		
			// digital io
			for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
				if (mod.io[i].digitalInput)
					mod.io[i].value = m_slot2DIO[i]->GetValue();
		}
	}
	
	m_controlInterface.Step();
}
	
void SimulationWindow::OnStep(wxCommandEvent &event)
{
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


signed char SimulationWindow::GetJoystickValue(double val)
{
	if (val < 0)
		return (signed char)(val * 128.0);
		
	return (signed char)(val * 127.0);
}

double SimulationWindow::ConvertJoystickByte(int value, int min, int max)
{
	// scale the result down
	double result = (double)value / ((double)(max - min)/2) - 1;

	if (result > 1.0)
		result = 1.0;
	else if (result < -1.0)
		result = -1.0;

	return result;
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
	
	m_statusBar->SetStatusText(wxString::Format(wxT("%.3f"), Simulator::GetTime()), 1);

	// then do the drawing
	
	// slot 4
	{
		DigitalModuleData &mod = m_controlInterface.simulationData.digitalModule[0];
	
		// pwm
		for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
		{
			m_slot1PWM[i]->SetEnabled( mod.pwm[i].pwm != NULL );
			m_slot1PWM[i]->SetValue( mod.pwm[i].speed );
		}

		// digital io
		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
		{
			bool en = true;
		
			if (mod.io[i].digitalInput)
				m_slot1DIO_lbl[i]->SetLabel(wxT("IN"));
			
			else if (mod.io[i].digitalOutput)
			{
				m_slot1DIO[i]->SetReadOnly(true);
				m_slot1DIO[i]->SetValue( mod.io[i].value );
				m_slot1DIO_lbl[i]->SetLabel(wxT("OUT"));
			}
			else
			{
				m_slot1DIO[i]->SetReadOnly(true);
				m_slot1DIO[i]->SetValue(false);

				if (mod.io[i].used)
					m_slot1DIO_lbl[i]->SetLabel(wxT("EN"));
				else
				{
					m_slot1DIO_lbl[i]->SetLabel(wxT("--"));
					en = false;
				}
			}
			
			m_slot1DIO[i]->SetEnabled(en);
		}
	}
	
	
	// slot 6
	{
		DigitalModuleData &mod = newData.digitalModule[1];
	
		// pwm
		for (size_t i = 0; i < DIGITAL_PWM_CHANNELS; i++)
		{
			m_slot2PWM[i]->SetEnabled( mod.pwm[i].pwm != NULL );
			m_slot2PWM[i]->SetValue( mod.pwm[i].speed );
		}

		// digital io
		for (size_t i = 0; i < DIGITAL_IO_CHANNELS; i++)
		{
			bool en = true;
		
			if (mod.io[i].digitalInput)
				m_slot2DIO_lbl[i]->SetLabel(wxT("IN"));
			
			else if (mod.io[i].digitalOutput)
			{
				m_slot2DIO[i]->SetReadOnly(true);
				m_slot2DIO[i]->SetValue( mod.io[i].value );
				m_slot2DIO_lbl[i]->SetLabel(wxT("OUT"));
			}
			else
			{
				m_slot2DIO[i]->SetReadOnly(true);
				m_slot2DIO[i]->SetValue(false);
				
				if (mod.io[i].used)
					m_slot2DIO_lbl[i]->SetLabel(wxT("EN"));
				else
				{
					m_slot2DIO_lbl[i]->SetLabel(wxT("--"));
					en = false;
				}
			}
			
			m_slot2DIO[i]->SetEnabled(en);
		}
	}
	
	
	// slot 8
	{
		SolenoidModule &mod = newData.solenoidModule;
	
		for (size_t i = 0; i < SOLENOID_IO_CHANNELS; i++)
		{
			if (mod.solenoids[i].solenoid)
			{
				m_solenoids[i]->SetValue( mod.solenoids[i].value );
				m_solenoids[i]->SetEnabled(true);
			}
			else
				m_solenoids[i]->SetEnabled(false);
		}
	}
	
	
	
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

void SimulationWindow::OnJoystick(wxJoystickEvent &event)
{
	if (!m_joy1_enable->IsChecked())
		return;

	wxPoint pt = m_joystick1.GetPosition();

	m_joy1_x->SetValue( ConvertJoystickByte(pt.x, m_joystick1.GetXMin(), m_joystick1.GetXMax()) );
	m_joy1_y->SetValue( ConvertJoystickByte(pt.y, m_joystick1.GetYMin(), m_joystick1.GetYMax()) );
	m_joy1_z->SetValue( ConvertJoystickByte(m_joystick1.GetZPosition(), m_joystick1.GetZMin(), m_joystick1.GetZMax()) );
	m_joy1_t->SetValue( ConvertJoystickByte(m_joystick1.GetRudderPosition(), m_joystick1.GetRudderMin(), m_joystick1.GetRudderMax()) );

	int buttons = m_joystick1.GetButtonState();

	m_joy1_1->SetValue( (buttons & 0x01) == 0x01);
	m_joy1_2->SetValue( (buttons & 0x02) == 0x02);
	m_joy1_3->SetValue( (buttons & 0x04) == 0x04);
	m_joy1_4->SetValue( (buttons & 0x08) == 0x08);

}

