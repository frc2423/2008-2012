
#include "ServoCalibrator.h"
#include "Framework/DriverStationLCD.h"


ServoCalibrator::ServoCalibrator(RobotChassis * chassis) :
	m_chassis(chassis),
	m_stick(Joystick::GetStickForPort(1)
{}


bool ServoCalibrator::ConvertStickToAngle(double &angle)
{
	double y = stick->GetY() * -1, x = stick->GetX();
	
	double speed = __hypot(x, y);
	
	double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 );			
	if (desired_angle < 0) desired_angle += 360;
	
	if (fabs(speed) < 0.01)
		return false;
	
	angle = desired_angle;
	return true;
}

ServoCalibrator::Calibrate()
{	
	KwarqsWheelServo * servo = NULL;

	int input = GetBCDInput();

	switch (input)
	{
	case 1:
		servo = &m_chassis->servo_lf;
		break;
		
	case 2:
		servo = &m_chassis->servo_lr;
		break;
		
	case 3:
		servo = &m_chassis->servo_rf;
		break;
		
	case 4:
		servo = &m_chassis->servo_rr;
		break;
		
	default:
		servo = NULL;
	}


	// do calibration
	if (servo)
	{
		double setPoint;
		
		if (stick.GetTop())
			servo->Calibrate();
			
		if (stick.GetTrigger())
			servo->EnableManualCalibration();
		else
			servo->DisableManualCalibration();
		
		// only change angle if desired
		if (ConvertStickToAngle(setPoint))
			servo->SetAngle(setPoint);
	}

	if (GetTime() - time > 0.1)
	{
		DriverStationLCD * lcd = DriverStationLCD::GetInstance();

		switch (input)
		{
		case 1:
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Left Front(LF)");
			break;
			
		case 2:
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Left Rear (LR)");
			break;
			
		case 3:
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Right Front (RF)");
			break;
			
		case 4:
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: Right Rear (RR)");
			break;
			
		default:
			lcd->PrintfLine(DriverStationLCD::kUser_Line2, "Sel: None (%d)", GetBCDInput());
		}
		
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line3, "LF: %.1f %.1f %s %s",
			m_chassis->servo_lf.GetSetAngle(),
			m_chassis->servo_lf.GetCurrentAngle(),
			m_chassis->servo_lf.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_lf.GetSensor() ? "0" : "1" 
		);
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line4, "LR: %.1f %.1f %s %s",
			m_chassis->servo_lr.GetSetAngle(),
			m_chassis->servo_lr.GetCurrentAngle(),
			m_chassis->servo_lr.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_lr.GetSensor() ? "0" : "1" 
		);
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line5, "RF: %.1f %.1f %s %s",
			m_chassis->servo_rf.GetSetAngle(),
			m_chassis->servo_rf.GetCurrentAngle(),
			m_chassis->servo_rf.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_rf.GetSensor() ? "0" : "1" 
		);

		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "RR: %.1f %.1f %s %s",
			m_chassis->servo_rr.GetSetAngle(),
			m_chassis->servo_rr.GetCurrentAngle(),
			m_chassis->servo_rr.IsCalibrated() ? "C" : "NC",
			m_chassis->servo_rr.GetSensor() ? "0" : "1" 
		);
		
		lcd->UpdateLCD();
		time = GetTime();
	}
}