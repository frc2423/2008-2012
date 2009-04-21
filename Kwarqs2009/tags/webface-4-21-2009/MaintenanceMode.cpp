/**
	\file 		MaintenanceMode.cpp
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

#include "KwarqsLib/KwarqsConstants.h"
#include "MaintenanceMode.h"

MaintenanceMode::MaintenanceMode(RobotChassis * chassis) :
	m_lcd(DriverStationLCD::GetInstance()),
	m_info(PositionInformation::GetInstance()),
	m_stick(FIRST_JOYSTICK_PORT),
	servoCalibrator(chassis),
	m_chassis(chassis),
	m_triggerEvent(0.5),
	
	m_helpScreen(1),
	last_user_selection(-1), 
	
	m_servoEvent(1),
	m_servoTestRunning(0),
	
	m_motorEvent(1)
{}

void MaintenanceMode::Reset()
{
	// stop anything that was started here... 
	switch (last_user_selection)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			servoCalibrator.Reset();
			break;

		case 7:
			m_chassis->servo_lf.Enable();
			m_chassis->servo_lr.Enable();
			m_chassis->servo_rf.Enable();
			m_chassis->servo_rr.Enable();
			break;
			
		case 8:
			break;

		default:
			break;
	}
}


void MaintenanceMode::DoMaintenance(int user_selection)
{
	// synchronize all lcd update events
	if (m_printEvent.DoEvent())
		print_to_lcd = true;
	else
		print_to_lcd = false;

	// if the mode switches, clean up any state left over
	if (user_selection != last_user_selection)
	{
		Reset();
		last_user_selection = user_selection;
	}

	switch (user_selection)
	{		
		case 1:
		
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "1. Auto Calibrate");
		
			// do the auto calibrate
			servoCalibrator.DoAutoCalibrate();
			break;
			
		case 2:
		
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "2. 4-pot Calibrate");
		
			// do the manual servo calibration
			servoCalibrator.DoManualCalibrate();
			break;
			
		case 3:
		
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "3. LF Servo");
		
			// do the manual servo calibration
			servoCalibrator.DoLFServo();
			break;
		
		case 4:
		
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "4. LR Servo");
		
			// do the manual servo calibration
			servoCalibrator.DoLRServo();
			break;
			
		case 5:
		
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "5. RF Servo");
		
			// do the manual servo calibration
			servoCalibrator.DoRFServo();
			break;
			
		case 6:
		
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "6. RR Servo");
		
			// do the manual servo calibration
			servoCalibrator.DoRRServo();
			break;
			
			
		case 7:
		
			// run the encoder test
			
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "7. Servo speed test");
			
			ServoSpeedTest();
			break;
			
		case 8:
		
			// run the wheel speed test
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "8. Wheel speed test");
			
			WheelSpeedTest();
			break;
			
		case 9:
		
			// display position-related information (gyro, accelerometer)
			if (print_to_lcd)
			{
				double ax, ay;
				m_info->GetAcceleration(ax, ay);
			
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line, "9. Position output");
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "Field @: %.1f (+%d)", m_info->GetNormalizedFieldAngle(), (int)m_info->GetFieldOffset());
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "Raw @: %.1f", m_info->GetNormalizedRawAngle());
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "accel X: %f", ax);
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line6, "accel Y: %f", ay);
			}
		
			break;
			
		default:
			
			if (print_to_lcd)
			{
				// delay the recognition of the switch
				if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
					m_helpScreen++;
				
				m_lcd->PrintfLine(DriverStationLCD::kMain_Line,  "Maintenance mode help");
				
				switch (m_helpScreen %3)
				{
				case 1:
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "1. Auto Calibrate");
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "2. 4-pot Calibrate");
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "3. LF Servo");
					break;
				
				case 2:
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "4. LR Servo");
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "5. RF Servo");
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "6. RR Servo");
					break;
				
				case 0:
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "7. Servo speed test");
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "8. Wheel speed test");
					m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "9. Position output");
					break;
					
				}
				
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line6, "  Trigger for next ->");
			}
			
			break;
	}
}





void MaintenanceMode::ServoSpeedTest()
{
	if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
	{
		m_lf_servo_vals.clear();
		m_lr_servo_vals.clear();
		m_rf_servo_vals.clear();
		m_rr_servo_vals.clear();
		m_servoTestRunning = 1;
	}
	
	/// @todo write a more detailed report to file instead
	
	// test state machine
	switch (m_servoTestRunning)
	{
		default:
			if (print_to_lcd)
			{
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "  Press trigger to   ");
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "     begin test      ");
			}
			
			m_chassis->servo_lf.Disable();
			m_chassis->servo_lr.Disable();
			m_chassis->servo_rf.Disable();
			m_chassis->servo_rr.Disable();
			
			break;
			
		case 1:
		case 2:
		case 3:
		case 4:
			
			if (print_to_lcd)
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "Please wait...");
			
			// runs every second
			if (m_servoEvent.DoEvent() )
			{
				m_lf_servo_vals.push_back(m_chassis->servo_lf.GetRawEncoder());
				m_lr_servo_vals.push_back(m_chassis->servo_lr.GetRawEncoder());
				m_rf_servo_vals.push_back(m_chassis->servo_rf.GetRawEncoder());
				m_rr_servo_vals.push_back(m_chassis->servo_rr.GetRawEncoder());
				
				m_chassis->servo_lf.SetRawMotor(1);
				m_chassis->servo_lr.SetRawMotor(1);
				m_chassis->servo_rf.SetRawMotor(1);
				m_chassis->servo_rr.SetRawMotor(1);
				
				++m_servoTestRunning;
			}
			
			break;
		
		case 5:
		
			// done, disable the servos
			m_chassis->servo_lf.SetRawMotor(0);
			m_chassis->servo_lr.SetRawMotor(0);
			m_chassis->servo_rf.SetRawMotor(0);
			m_chassis->servo_rr.SetRawMotor(0);
			
			++m_servoTestRunning;

			// fall through to the next case
			
		case 6:
			
			if (print_to_lcd)
			{
				// print the measured distance
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line3, "LF: %d %d %d", 
					m_lf_servo_vals[1] - m_lf_servo_vals[0], 
					m_lf_servo_vals[2] - m_lf_servo_vals[1], 
					m_lf_servo_vals[3] - m_lf_servo_vals[2] 
				);
				
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line4, "LR: %d %d %d",
					m_lr_servo_vals[1] - m_lr_servo_vals[0], 
					m_lr_servo_vals[2] - m_lr_servo_vals[1], 
					m_lr_servo_vals[3] - m_lr_servo_vals[2] 
				);
				
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line5, "RF: %d %d %d",
					m_rf_servo_vals[1] - m_rf_servo_vals[0], 
					m_rf_servo_vals[2] - m_rf_servo_vals[1], 
					m_rf_servo_vals[3] - m_rf_servo_vals[2] 
				);
				m_lcd->PrintfLine(DriverStationLCD::kUser_Line6, "RR: %d %d %d",
					m_rr_servo_vals[1] - m_rr_servo_vals[0], 
					m_rr_servo_vals[2] - m_rr_servo_vals[1], 
					m_rr_servo_vals[3] - m_rr_servo_vals[2] 
				);
			}
			break;
	}
}

void MaintenanceMode::WheelSpeedTest()
{
	//if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
}



