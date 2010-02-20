#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"
#include "Vision.h"
#include "CompassMode.h"
#include "PSUSBMode.h"
#include "AutonomousVisionMode.h"
#include "Kicker.h"
#include "EncoderMode.h"
#include "Latches.h"

#include <limits>
#undef min
#undef max

// this just exists to make debugging easier
struct StackEnable {
	StackEnable()
	{
		wpi_stackTraceEnable(true);
	}
};

class RobotDemo : public SimpleRobot
{
	static const int VISION_LEFT_BUTTON = 4;
	static const int VISION_RIGHT_BUTTON = 5;
	static const int VISION_EITHER_BUTTON = 3;



	StackEnable unused;
	
	RobotResources resources;
	ExampleMode example;
	Vision vision;
	CompassMode compass;
	Kicker kicker;
	AutonomousVisionMode autonomousVision;
	EncoderMode encoderMode;
	Mode mode;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		kicker(resources),
		autonomousVision(resources, kicker, vision, 1),
		encoderMode(resources),
		mode(&autonomousVision)
	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
		//mode.Add(&compass);
		//mode.Add(&encoderMode);
		
		kicker.Start();
	}

	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		
		while(IsAutonomous())
		{
			mode.Autonomous();
			// DO NOT TAKE THIS OUT
			Wait(0.005);
		}
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		
		TimedLatch eitherButton;
		TimedLatch leftVision;
		TimedLatch rightVision;
		
		// this state variable keeps track of who is supposed to be
		// touching the motors right now
		enum {	NO_BALL, NO_TARGET, AUTO_KICK } autoTarget_state = NO_BALL;
		enum { VB_LEFT, VB_RIGHT, VB_EITHER, VB_NONE } motor_state = VB_NONE;
		
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if( resources.stick.GetTrigger() ) kicker.Kick();
			
			// update the buttons each time around
			leftVision.Set( resources.stick.GetRawButton(VISION_LEFT_BUTTON));
			rightVision.Set( resources.stick.GetRawButton(VISION_RIGHT_BUTTON));
			eitherButton.Set(resources.stick.GetRawButton(VISION_EITHER_BUTTON));
			
			if(DriverStation::GetInstance()->GetDigitalIn(3))
			{	
				
				switch (autoTarget_state)
				{
				case NO_BALL:
					mode.run();
					if(kicker.HasBall())
					{
						
						vision.PreferEither();
						motor_state = VB_EITHER;
						autoTarget_state = NO_TARGET;
					}
					break;
				case NO_TARGET:
					if(vision.IsRobotPointingAtTarget()) 
					{
						vision.DisableMotorControl();
						autoTarget_state = AUTO_KICK;
					}
					break;
				case AUTO_KICK:
					kicker.Kick();
					autoTarget_state = NO_BALL;
					break;
				}
			}
			else
			{
				//returns autoTarget to first state if autoTarget is turned off
				//in middle of 
				autoTarget_state = NO_BALL;
				// this switch statement decides who controls the motors
				switch (motor_state)
				{
					case VB_LEFT:
						
						if (leftVision.TurnedOff())
						{
							vision.DisableMotorControl();
							motor_state = VB_NONE;
						}
						
						break;
					
					case VB_RIGHT:
					
						if (rightVision.TurnedOff())
						{
							vision.DisableMotorControl();
							motor_state = VB_NONE;
						}
						
						break;
					
					case VB_EITHER:
	
						if (eitherButton.TurnedOff())
						{
							vision.DisableMotorControl();
							motor_state = VB_NONE;
						}
					
						break;
					
					case VB_NONE:
					
						// only do the transition here if no other
						// button is selected, otherwise accidentally
						// hitting two buttons may disable the targeting
						
						if (eitherButton.TurnedOn())
						{
							vision.PreferEither();
							motor_state = VB_EITHER;
						}
						else if (leftVision.TurnedOn())
						{
							vision.PreferLeft();
							motor_state = VB_LEFT;
						}
						else if (rightVision.TurnedOn())
						{
							vision.PreferRight();
							motor_state = VB_RIGHT;
						}
						else
						{
							// do normal things here
							mode.run();
						}
					
						break;		
				}
			}
			
			// DO NOT TAKE THIS OUT
			Wait(0.005);
		}
		
		// disable the vision controlling if we exit operator control
		vision.DisableMotorControl();
	}
};

START_ROBOT_CLASS(RobotDemo);

