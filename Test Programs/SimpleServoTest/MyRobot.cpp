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
	Joystick stick2;

	Encoder encoder;
	Encoder motorEncoder;
	DigitalInput sensor;
	Victor victor;
	Jaguar jaguar;

#define SLOT SLOT_2
	
public:
	RobotDemo(void):
		stick(1), stick2(2),		// as they are declared above.
		encoder(SLOT, 3, SLOT, 4),
		motorEncoder(SLOT, 7, SLOT, 8, false, Encoder::k2X),
		sensor(SLOT, 10),
		victor(SLOT, 5),
		jaguar(SLOT, 3)
	{
		GetWatchdog().SetExpiration(0.1);
		encoder.Start();
		motorEncoder.Start();
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
			jaguar.Set(stick2.GetY()*-1);
			
			if (GetTime() - time > 0.025)
			{
				
				double angle = fmod(
						
				(((double)((encoder.GetRaw() - 0) % 3272) * 360.0) 
							/ (double)3272) - 0, 360.0);
							
				if (angle < 0.0)
					angle += 360.0;
					
				printf("Encoder: %d Motor: %d Sensor: %d, Angle: %f\r",
						encoder.Get(), motorEncoder.Get(), sensor.Get(), angle);
				time = GetTime();
			}
			
			
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

