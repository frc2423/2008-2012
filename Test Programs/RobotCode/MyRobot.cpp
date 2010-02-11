#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"

#include <limits>
#undef min
#undef max

class RobotDemo : public SimpleRobot
{
	RobotResources resources;
	ExampleMode example;
	Mode mode;


public:
	RobotDemo(void):
		example(resources)
	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		resources.myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		resources.myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			//mode switch code goes here
			

		}
	}
};

START_ROBOT_CLASS(RobotDemo);

