/**
	\file 		MaintenanceMode.cpp
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-01 04:12:01 -0500 (Sun, 01 Mar 2009) $
	\version 	$Rev: 251 $
*/


#include "KwarqsLib/KwarqsConstants.h"
#include "MaintenanceMode.h"

DelayEvent lcdEvent;
DelayEvent triggerDetect;

MaintenanceMode::MaintenanceMode(RobotChassis * chassis) :
	lcd(DriverStationLCD::GetInstance()),
	m_info(PositionInformation::GetInstance()),
	m_stick(FIRST_JOYSTICK_PORT),
	servoCalibrator(chassis),
	m_triggerEvent(0.5),
	m_helpScreen(1)
{}

void MaintenanceMode::Reset()
{
	// stop anything that was started here... 
	servoCalibrator.Reset();
	
	// if m_servoTestRunning  -- re-enable servos
}


void MaintenanceMode::DoMaintenance(int user_selection)
{
	switch (user_selection)
	{		
		case 1:
		
			if (m_lcdEvent->DoEvent())
				lcd->PrintfLine(kMain_Line, "1. Auto Calibrate");
		
			// do the auto calibrate
			servoCalibrator.DoAutoCalibrate();
			break;
			
		case 2:
		
			if (m_lcdEvent->DoEvent())
				lcd->PrintfLine(kMain_Line, "2. 4-pot Calibrate");
		
			// do the manual servo calibration
			servoCalibrator.DoManualCalibrate();
			break;
			
		case 3:
		
			// run the encoder test
			
			if (m_lcdEvent->DoEvent())
				lcd->PrintfLine(kMain_Line, "3. Servo speed test");
			
			EncoderTest();
			break;
			
		case 4:
		
			// run the wheel speed test
			if (m_lcdEvent->DoEvent())
				lcd->PrintfLine(kMain_Line, "4. Wheel speed test");
			
			WheelSpeedTest();
			break;
			
		case 5:
		
			// display position-related information (gyro, accelerometer)
			if (m_lcdEvent->DoEvent())
			{
				double ax, ay;
				info->GetAcceleration(ax, ay);
			
				lcd->PrintfLine(kMain_Line, "5. Position output");
				lcd->PrintfLine(kUser_Line3, "Field @: %.1f", info->GetNormalizedFieldAngle());
				lcd->PrintfLine(kUser_Line4, "Raw @: %.1f", info->GetNormalizedRawAngle());
				lcd->PrintfLine(kUser_Line5, "accel X: %f", ax);
				lcd->PrintfLine(kUser_Line6, "accel Y: %f", ay);
			}
		
			break;
			
		default:
			
			if (m_lcdEvent->DoEvent())
			{
				// delay the recognition of the switch
				if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
					helpScreen++;
				
				lcd->PrintfLine(kMain_Line,  "Maintenance mode help");
				
				if (helpScreen % 2)
				{
					lcd->PrintfLine(kUser_Line3, "1. Auto Calibrate");
					lcd->PrintfLine(kUser_Line4, "2. 4-pot Calibrate");
					lcd->PrintfLine(kUser_Line5, "3. Servo speed test");
				}
				else
				{
					lcd->PrintfLine(kUser_Line3, "4. Wheel speed test");
					lcd->PrintfLine(kUser_line4, "5. Position output");
				}
				
				lcd->Printf(kUser_Line6, "  Trigger for next ->"
			}
			
			break;
	}
}

void MaintenanceMode::WheelSpeedTest()
{
	//if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
}

void MaintenanceMode:ServoSpeedTest()
{
/*
	if (m_stick.GetTrigger() && m_triggerEvent.DoEvent())
		m_servoTestRunning = 1;
		
	switch (m_servoTestRunning)
	{
		
	
	}
		
	if (m_servoTestRunning)
	{
		// start all the servos
		
		// then when its time, 
	
	
	}
	
			lcd->Printf(DriverStationLCD::kUser_Line3, 1, "%d %d %d %d",
			servo_lf.GetRaw(),
			servo_lr.GetRaw(),
			servo_rf.GetRaw(),
			servo_rr.GetRaw() 
		);
		
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if (GetTime() - start_time < 5)
			{
				servo_lf.SetRaw(1);
				servo_lr.SetRaw(1);
				servo_rf.SetRaw(1);
				servo_rr.SetRaw(1);
			}
			else
			{
				servo_lf.SetRaw(0);
				servo_lr.SetRaw(0);
				servo_rf.SetRaw(0);
				servo_rr.SetRaw(0);
				
				lcd->Printf(DriverStationLCD::kUser_Line3, 1, "%d %d %d %d",
					servo_lf.GetRaw(),
					servo_lr.GetRaw(),
					servo_rf.GetRaw(),
					servo_rr.GetRaw() 
				);
				
				lcd->UpdateLCD();
			}
		}
	
	
*/
	
}



