#include "WPILib.h"

#include <math.h>

#define SLOT_1	4
#define SLOT_2	6

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	Joystick stick; // only joystick

	Encoder encoder;
	DigitalInput sensor;
	Victor victor;

public:
	RobotDemo(void):
		stick(1),		// as they are declared above.
		encoder(SLOT_2, 3, SLOT_2, 4),
		sensor(SLOT_2, 10),
		victor(SLOT_2, 4)
	{
		GetWatchdog().SetExpiration(0.1);
		encoder.Start();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		printf("OperatorControl()\n");
		double time = GetTime();
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			victor.Set(stick.GetY()*-1);
			
			if (GetTime() - time > 0.025)
			{
				
				double angle = fmod(
						
				(((double)((encoder.GetRaw() - 0) % 3272) * 360.0) 
							/ (double)3272) - 0, 360.0);
							
				if (angle < 0.0)
					angle += 360.0;
					
					
				
				printf("Encoder: %d. Sensor: %d, Angle: %f\r",
						encoder.Get(), sensor.Get(), angle);
				time = GetTime();
			}
			
			
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

