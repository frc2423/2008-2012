#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"
#include "VisionMode.h"
#include "CompassMode.h"
#include "AutonomousMode.h"
#include "PSUSBMode.h"
#include "AutonomousVisionMode.h"

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
	PSUSBMode PSUSB;
	AutonomousVisionMode autonomousVision;
	Mode mode;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		autonomous(resources),
		PSUSB(resources),
		autonomousVision(resources, 1),
		mode(&autonomousVision)

	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
		mode.Add(&vision);
		mode.Add(&compass);
		mode.Add(&PSUSB);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		mode.Autonomous();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		
		bool prev_Previous = false;
		bool prev_Next = false;
		bool prev_Trigger = false;
		int stored_mode = 0;
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			//mode switch code
			
			if(resources.stick.GetRawButton(4) && !prev_Previous)
			{	
				mode.Previous();
				prev_Previous = true;
			}
			else if(!resources.stick.GetRawButton(4) && prev_Previous)
			{
				prev_Previous = false;
			}		
			
			if(resources.stick.GetRawButton(5) && !prev_Next)
			{	
				mode.Next();
				prev_Next = true;
			}
			else if(!resources.stick.GetRawButton(5) && prev_Next)
			{
				prev_Next = false;
			}
			
			
			if(resources.stick.GetRawButton(3) && !prev_Trigger)
			{
				stored_mode = mode.GetMode();
				prev_Trigger = true;
				mode.Set(1);
			}
			else if(!resources.stick.GetRawButton(3) && prev_Trigger)
			{
				mode.Set(stored_mode);
				prev_Trigger = false;
			}
			else
			{
				mode.run();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

