#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"
#include "VisionMode.h"
#include "CompassMode.h"
#include "AutonomousMode.h"

#include <limits>
#undef min
#undef max

class RobotDemo : public SimpleRobot
{
	RobotResources resources;
	ExampleMode example;
	VisionMode vision;
	CompassMode compass;
	AutonomousMode autonomous;
	Mode mode;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		autonomous(resources),
		mode(&autonomous)

	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
		mode.Add(&vision);
		mode.Add(&compass);
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
		
		bool prev_trigger = false;
		int stored_mode = 0;
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			//mode switch code
			
			if(resources.stick.GetTop())
			{
				mode.Next();
			}
			if(resources.stick.GetTrigger() && !prev_trigger)
			{
				stored_mode = mode.GetMode();
				prev_trigger = true;
				mode.Set(1);
			}
			else if(!resources.stick.GetTrigger() && prev_trigger)
			{
				mode.Set(stored_mode);
				prev_trigger = false;
			}
			else
			{
				mode.run();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

