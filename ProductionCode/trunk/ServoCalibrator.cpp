/**
	\file 		ServoCalibrator.cpp
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-02-16 09:05:51 -0500 (Mon, 16 Feb 2009) $
	\version 	$Rev: 166 $
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
#include "Framework/DriverStationLCD.h"
#include "Framework/KwarqsBCDInput.h"
#include "Framework/KwarqsConstants.h"
#include "Framework/math.h"

ServoCalibrator::ServoCalibrator(RobotChassis * chassis) :
	m_stick(FIRST_JOYSTICK_PORT),
	m_motorStick(SECOND_JOYSTICK_PORT),
	m_chassis(chassis),
	m_time(0)
	//m_in_pot_mode(false),
	//m_idx(0),
	//m_pot_time(0)
{
	//for (int i = 0; i < POT_ARRAY_SIZE; i++)
	//	m_pot_array[i] = 0;
}


bool ServoCalibrator::ConvertStickToAngle(double &angle)
{
	double y = m_stick.GetY() * -1, x = m_stick.GetX();
	
	double speed = __hypot(x, y);
	
	double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 );			
	if (desired_angle < 0) desired_angle += 360;
	
	if (fabs(speed) < 0.01)
		return false;
	
	angle = desired_angle;
	return true;
}

void ServoCalibrator::Calibrate()
{	
	KwarqsWheelServo * servo = NULL;
	KwarqsWheelMotor * motor = NULL;

	int input = GetBCDInput();

	switch (input)
	{
	case 1:
		servo = &m_chassis->servo_lf;
		motor = &m_chassis->motor_lf;
		break;
		
	case 2:
		servo = &m_chassis->servo_lr;
		motor = &m_chassis->motor_lr;
		break;
		
	case 3:
		servo = &m_chassis->servo_rf;
		motor = &m_chassis->motor_rf;
		break;
		
	case 4:
		servo = &m_chassis->servo_rr;
		motor = &m_chassis->motor_rr;
		break;
		
	default:
		servo = NULL;
	}


	// do calibration
	if (servo)
	{
		double setPoint;
		
		if (m_stick.GetTop())
			servo->Calibrate();
		
		if (m_stick.GetTrigger())
		{
			// adjust using the pot, filter the values
			//if (GetTime() - m_pot_time > 0.1)
			//{
				servo->SetAngle( ceil( GetFilteredPotValue() /(1000.0/ 500.0) ) );
			//	m_pot_time = GetTime();
			//}
			
			m_in_pot_mode = true;
		} 
		else if (m_in_pot_mode)
		{
			servo->Reset();
			m_in_pot_mode = false;
		}
		 // only change angle if desired
		else if (ConvertStickToAngle(setPoint))
			servo->SetAngle(setPoint);
		
		motor->SetSpeed(m_motorStick.GetY()*-1);
	}

	if (GetTime() - m_time > 0.1)
	{
		DriverStationLCD * lcd = DriverStationLCD::GetInstance();

		switch (input)
		{
		case 1:
			lcd->PrintfLine(DriverStationLCD::kMain_Line6, "Sel: Left Front(LF)  ");
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Left Front(LF)  ");
			break;
			
		case 2:
			lcd->PrintfLine(DriverStationLCD::kMain_Line6, "Sel: Left Rear (LR)  ");
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Left Rear (LR)  ");
			break;
			
		case 3:
			lcd->PrintfLine(DriverStationLCD::kMain_Line6, "Sel: Right Front (RF)");
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Right Front (RF)");
			break;
			
		case 4:
			lcd->PrintfLine(DriverStationLCD::kMain_Line6, "Sel: Right Rear (RR) ");
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Right Rear (RR) ");
			break;
			
		default:
			lcd->PrintfLine(DriverStationLCD::kMain_Line6, "Sel: None (%d)       ", GetBCDInput());
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: None (%d)       ", GetBCDInput());
		}
		
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line3, "LF: %.1f %.1f %s %s            ",
			m_chassis->servo_lf.GetSetAngle(),
			m_chassis->servo_lf.GetCurrentAngle(),
			m_chassis->servo_lf.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_lf.GetSensor() ? "0" : "1" 
		);
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line4, "LR: %.1f %.1f %s %s            ",
			m_chassis->servo_lr.GetSetAngle(),
			m_chassis->servo_lr.GetCurrentAngle(),
			m_chassis->servo_lr.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_lr.GetSensor() ? "0" : "1" 
		);
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line5, "RF: %.1f %.1f %s %s            ",
			m_chassis->servo_rf.GetSetAngle(),
			m_chassis->servo_rf.GetCurrentAngle(),
			m_chassis->servo_rf.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_rf.GetSensor() ? "0" : "1" 
		);

		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "RR: %.1f %.1f %s %s            ",
			m_chassis->servo_rr.GetSetAngle(),
			m_chassis->servo_rr.GetCurrentAngle(),
			m_chassis->servo_rr.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_rr.GetSensor() ? "0" : "1" 
		);
		
		lcd->UpdateLCD();
		m_time = GetTime();
	}
}

double ServoCalibrator::GetFilteredPotValue()
{
	return DriverStation::GetInstance()->GetAnalogIn(1);
	
	/*
	if (++m_idx > POT_ARRAY_SIZE)
		m_idx = 0;
	
	m_pot_array[m_idx] = DriverStation::GetInstance()->GetAnalogIn(1);
	
	// average it
	double sum = 0;
	for (int i = 0; i < POT_ARRAY_SIZE; i++)
		sum += m_pot_array[i];
	
	return sum/POT_ARRAY_SIZE;
	*/
}
