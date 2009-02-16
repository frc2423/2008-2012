#include "WPILib.h"

#include <math.h>
#include "Framework/KwarqsWheelServo.h"
#include "Framework/KwarqsConstants.h"
#include "Framework/math.h"
#include "Framework/DriverStationLCD.h"

#include "Framework/KwarqsBCDInput.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	Joystick 			stick; 		// only joystick
	
	DriverStationLCD	* lcd;
	
	KwarqsWheelServo servo_lf, servo_lr, servo_rf, servo_rr;
	
	
	
public:
	// slot, pwm, e1, e2, cal
	RobotDemo(void):
		stick(1),
		servo_lf(SERVO_LF_PARAMETERS),
		servo_lr(SERVO_LR_PARAMETERS),
		servo_rf(SERVO_RF_PARAMETERS),
		servo_rr(SERVO_RR_PARAMETERS)
	{
		GetWatchdog().SetExpiration(100);
		
		//servo_lf.Calibrate();
		//servo_lr.Calibrate();
		//servo_rf.Calibrate();
		//servo_rr.Calibrate();		
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
	}
	
	bool ConvertStickToAngle(double &angle)
	{
		double y = stick.GetY() * -1, x = stick.GetX();
		
		double speed = __hypot(x, y);
		
		double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 );			
		if (desired_angle < 0) desired_angle += 360;
		
		if (fabs(speed) < 0.01)
			return false;
		
		angle = desired_angle;
		return true;
	}
	
	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		
		double time = GetTime();
		
		lcd = DriverStationLCD::GetInstance();
		lcd->Printf(DriverStationLCD::kMain_Line6, 1, "Hit select for status");
		
		
		printf("Entered OperatorControl()\n");
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			KwarqsWheelServo * servo = NULL;
			
			int input = GetBCDInput();
			
			switch (input)
			{
			case 1:
				servo = &servo_lf;
				break;
				
			case 2:
				servo = &servo_lr;
				break;
				
			case 3:
				servo = &servo_rf;
				break;
				
			case 4:
				servo = &servo_rr;
				break;
				
			default:
				servo = NULL;
			}
			
			
			// do calibration
			if (servo)
			{
				double setPoint;
				
				if (stick.GetTrigger())
					servo->Calibrate();
				
				// only change angle if desired
				if (ConvertStickToAngle(setPoint))
					servo->SetAngle(setPoint);
			}
			
			if (GetTime() - time > 0.1)
			{
				switch (input)
				{
				case 1:
					lcd->Printf(DriverStationLCD::kUser_Line2, 1, "Sel: Left Front(LF)");
					break;
					
				case 2:
					lcd->Printf(DriverStationLCD::kUser_Line2, 1, "Sel: Left Rear (LR)");
					break;
					
				case 3:
					lcd->Printf(DriverStationLCD::kUser_Line2, 1, "Sel: Right Front (RF)");
					break;
					
				case 4:
					lcd->Printf(DriverStationLCD::kUser_Line2, 1, "Sel: Right Rear (RR)");
					break;
					
				default:
					lcd->Printf(DriverStationLCD::kUser_Line2, 1, "Sel: None (%d)", GetBCDInput());
				}
				
				
				lcd->Printf(DriverStationLCD::kUser_Line3, 1, "LF: %.1f %.1f %s %s",
					servo_lf.GetSetAngle(),
					servo_lf.GetCurrentAngle(),
					servo_lf.IsCalibrated() ? "C" : "NC",
					servo_lf.GetSensor() ? "0" : "1" 
				);
				
				lcd->Printf(DriverStationLCD::kUser_Line4, 1, "LR: %.1f %.1f %s %s",
					servo_lr.GetSetAngle(),
					servo_lr.GetCurrentAngle(),
					servo_lr.IsCalibrated() ? "C" : "NC",
					servo_lr.GetSensor() ? "0" : "1" 
				);
				
				lcd->Printf(DriverStationLCD::kUser_Line5, 1, "RF: %.1f %.1f %s %s",
					servo_rf.GetSetAngle(),
					servo_rf.GetCurrentAngle(),
					servo_rf.IsCalibrated() ? "C" : "NC",
					servo_rf.GetSensor() ? "0" : "1" 
				);

				lcd->Printf(DriverStationLCD::kUser_Line6, 1, "RR: %.1f %.1f %s %s",
					servo_rr.GetSetAngle(),
					servo_rr.GetCurrentAngle(),
					servo_rr.IsCalibrated() ? "C" : "NC",
					servo_rr.GetSensor() ? "0" : "1" 
				);
				
				lcd->UpdateLCD();
				time = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

