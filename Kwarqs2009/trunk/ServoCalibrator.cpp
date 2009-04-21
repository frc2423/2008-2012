/**
	\file 		ServoCalibrator.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

/*
 * Copyright (c) 2009 Dustin Spicuzza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Dustin Spicuzza nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DUSTIN SPICUZZA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DUSTIN SPICUZZA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <WPILib.h>

#include "ServoCalibrator.h"
#include "KwarqsLib/KwarqsConstants.h"
#include "KwarqsLib/math.h"

ServoCalibrator::ServoCalibrator(RobotChassis * chassis) :
	m_stick(FIRST_JOYSTICK_PORT),
	m_chassis(chassis),
	m_ds(DriverStation::GetInstance()),
	
	m_lcd(DriverStationLCD::GetInstance()),
	m_triggerEvent(0.5),
	pot1offset(0),
	pot2offset(0),
	pot3offset(0),
	pot4offset(0),
	m_begin_manual_calibrate(true),
	last_trig(false)
{
	proxy_lf = WebInterface::CreateDoubleProxy("Manual", "LF Wheel", 
			DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(0.1));
	proxy_lr = WebInterface::CreateDoubleProxy("Manual", "LR Wheel", 
			DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(0.1));
	proxy_rf = WebInterface::CreateDoubleProxy("Manual", "RF Wheel", 
			DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(0.1));
	proxy_rr = WebInterface::CreateDoubleProxy("Manual", "RR Wheel", 
			DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(0.1));
}



void ServoCalibrator::DoAutoCalibrate()
{
	m_chassis->servo_lf.SetAngle(0);
	m_chassis->servo_lr.SetAngle(0);
	m_chassis->servo_rf.SetAngle(0);
	m_chassis->servo_rr.SetAngle(0);

	// debounce this input
	if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
	{
		m_chassis->servo_lf.AutoCalibrate();
		m_chassis->servo_lr.AutoCalibrate();
		m_chassis->servo_rf.AutoCalibrate();
		m_chassis->servo_rr.AutoCalibrate();
	}
	
	ShowLCDOutput();
}

void ServoCalibrator::DoManualCalibrate()
{	
	// filter out the pots
	/*
	const double c = 500.0 / 1000.0;

	double pot1 = ceil( m_ds->GetAnalogIn(1) * c);
	double pot2 = ceil( m_ds->GetAnalogIn(2) * c);
	double pot3 = ceil( m_ds->GetAnalogIn(3) * c);
	double pot4 = ceil( m_ds->GetAnalogIn(4) * c);


	// make sure the pots don't initially move the wheels around,
	// that gets annoying really fast. We use the recent value
	// instead of the filtered value because its more likely
	// to be correct. Within the percentage of noise, of course :)
	if (m_begin_manual_calibrate)
	{
		pot1offset = pot1 - m_chassis->servo_lf.GetSetAngle();
		pot2offset = pot2 - m_chassis->servo_lr.GetSetAngle();
		pot3offset = pot3 - m_chassis->servo_rf.GetSetAngle();
		pot4offset = pot4 - m_chassis->servo_rr.GetSetAngle();
		
		m_begin_manual_calibrate = false;
	}
				
	m_potfilter1.AddPoint( pot1 );
	m_potfilter2.AddPoint( pot2 );
	m_potfilter3.AddPoint( pot3 );
	m_potfilter4.AddPoint( pot4 );

	// hitting the trigger sets the calibration point
	if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
	{
		m_chassis->servo_lf.Reset();
		m_chassis->servo_lr.Reset();
		m_chassis->servo_rf.Reset();
		m_chassis->servo_rr.Reset();
		
		pot1offset = m_potfilter1.GetAverage();
		pot2offset = m_potfilter2.GetAverage();
		pot3offset = m_potfilter3.GetAverage();
		pot4offset = m_potfilter4.GetAverage();
	}
	
	// adjust the pot input accordingly
	pot1 = m_potfilter1.GetAverage() - pot1offset;
	pot2 = m_potfilter2.GetAverage() - pot2offset;
	pot3 = m_potfilter3.GetAverage() - pot3offset;
	pot4 = m_potfilter4.GetAverage() - pot4offset;
	
	// finally tell the servo to go to that angle
	*/
	
	
	m_chassis->servo_lf.SetAngle(proxy_lf);
	m_chassis->servo_lr.SetAngle(proxy_lr);
	m_chassis->servo_rf.SetAngle(proxy_rf);
	m_chassis->servo_rr.SetAngle(proxy_rr);
	
	
	ShowLCDOutput();
}

void ServoCalibrator::DoLFServo()
{
	DoServo(m_chassis->servo_lf);
}

void ServoCalibrator::DoLRServo()
{
	DoServo(m_chassis->servo_lr);
}

void ServoCalibrator::DoRFServo()
{
	DoServo(m_chassis->servo_rf);
}

void ServoCalibrator::DoRRServo()
{
	DoServo(m_chassis->servo_rr);
}


void ServoCalibrator::DoServo(KwarqsWheelServo &servo)
{
	
	bool trig = m_stick.GetTrigger();
			
	if (last_trig != trig)
	{
		if (trig)
			servo.EnableManual();
		else
		{
			// reset the servo params on the transition
			servo.Reset();
			servo.Enable();
			servo.SetAngle(0);
		}
		
		last_trig = trig;
	}
	
	if (trig)
		servo.SetRawMotor(m_stick.GetY()*-1);
	else if (m_stick.GetTop())
	{
		double y = m_stick.GetY() * -1, x = m_stick.GetX();
		double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 );
		
		servo.SetAngle(desired_angle);
	}

	ShowLCDOutput();
}


void ServoCalibrator::ShowLCDOutput()
{
	if (m_lcdEvent.DoEvent())
	{
		m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "LF: %s %s %5.1f %5.1f",
			m_chassis->servo_lf.IsCalibrated() ? "OK " : "CAL",
			m_chassis->servo_lf.GetSensor() ? "0" : "1",
			m_chassis->servo_lf.GetSetAngle(),
			m_chassis->servo_lf.GetCurrentAngle()
		);
		
		m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "LR: %s %s %5.1f %5.1f",
			m_chassis->servo_lr.IsCalibrated() ? "OK " : "CAL",
			m_chassis->servo_lr.GetSensor() ? "0" : "1",
			m_chassis->servo_lr.GetSetAngle(),
			m_chassis->servo_lr.GetCurrentAngle()
		);
		
		m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "RF: %s %s %5.1f %5.1f",
			m_chassis->servo_rf.IsCalibrated() ? "OK " : "CAL",
			m_chassis->servo_rf.GetSensor() ? "0" : "1",
			m_chassis->servo_rf.GetSetAngle(),
			m_chassis->servo_rf.GetCurrentAngle()
		);
		
		m_lcd->PrintfLine(DriverStationLCD::kUser_Line6, "RR: %s %s %5.1f %5.1f",
			m_chassis->servo_rr.IsCalibrated() ? "OK " : "CAL",
			m_chassis->servo_rr.GetSensor() ? "0" : "1",
			m_chassis->servo_rr.GetSetAngle(),
			m_chassis->servo_rr.GetCurrentAngle()
		);
	}
}


void ServoCalibrator::Reset()
{
	m_begin_manual_calibrate = true;
	
	if (!m_chassis->servo_lf.IsCalibrated())
		m_chassis->servo_lf.Reset();
	if (!m_chassis->servo_lr.IsCalibrated())
		m_chassis->servo_lr.Reset();
	if (!m_chassis->servo_rf.IsCalibrated())
		m_chassis->servo_rf.Reset();
	if (!m_chassis->servo_rr.IsCalibrated())
		m_chassis->servo_rr.Reset();
}
