#include "WPILib.h"

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
		encoder(SLOT_1, 3,4),
		sensor(SLOT_1, 5),
		victor(SLOT_1, 1)
	{
		GetWatchdog().SetExpiration(0.1);
		encoder.Start();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double time = GetTime();
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			victor.Set(stick.GetY()*-1);
			
			if (GetTime() - time > 0.025)
			{
				printf("Encoder: %d. Sensor: %d\r",
						encoder.Get(), sensor.Get());
				time = GetTime();
			}
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

