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
	
	
	
	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		
		double start_time = GetTime();
		
		lcd = DriverStationLCD::GetInstance();

		
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
	}
};

START_ROBOT_CLASS(RobotDemo);

