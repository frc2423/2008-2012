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
	m_begin_manual_calibrate(true)
{}



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
	// make sure the pots don't initially move the wheels around,
	// that gets annoying really fast
	if (m_begin_manual_calibrate)
	{
		pot1offset = m_chassis->servo_lf.GetSetAngle();
		pot2offset = m_chassis->servo_lr.GetSetAngle();
		pot3offset = m_chassis->servo_rf.GetSetAngle();
		pot4offset = m_chassis->servo_rr.GetSetAngle();
		
		m_begin_manual_calibrate = false;
	}

	// filter out the pots
	const double c = 500.0 / 1000.0;
				
	m_potfilter1.AddPoint( ceil( m_ds->GetAnalogIn(1) * c));
	m_potfilter2.AddPoint( ceil( m_ds->GetAnalogIn(2) * c));
	m_potfilter3.AddPoint( ceil( m_ds->GetAnalogIn(3) * c));
	m_potfilter4.AddPoint( ceil( m_ds->GetAnalogIn(4) * c));

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
	double pot1 = m_potfilter1.GetAverage() - pot1offset;
	double pot2 = m_potfilter2.GetAverage() - pot2offset;
	double pot3 = m_potfilter3.GetAverage() - pot3offset;
	double pot4 = m_potfilter4.GetAverage() - pot4offset;
	
	// ok, tell the servo to go to that angle
	m_chassis->servo_lf.SetAngle(pot1);
	m_chassis->servo_lr.SetAngle(pot2);
	m_chassis->servo_rf.SetAngle(pot3);
	m_chassis->servo_rr.SetAngle(pot4);
	
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
