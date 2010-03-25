/**
	\file 		MyRobot.cpp
	\author 	Amory Galili, Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/



#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"
#include "Vision.h"
#include "AutonomousVisionMode.h"
#include "Kicker.h"
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

	enum { VB_LEFT, VB_RIGHT, VB_EITHER, VB_NONE } m_motor_state;
	enum AutoTargetState { AT_NONE, AT_ACQUIRE_BALL, AT_ACQUIRE_TARGET };

	StackEnable unused;
	
	RobotResources resources;
	
	NosePointer 			m_nosePointer;
	PositionInformation		m_position;
	
	ExampleMode example;
	Vision vision;
	Kicker kicker;
	AutonomousVisionMode autonomousVision;
	
	ModeControl modeControl;
	ModeControl autonomousModeControl;
	
	TimedLatch eitherButton;
	TimedLatch leftVision;
	TimedLatch rightVision;
	
	TimedLatch autoKickButton;
	
	DelayEvent ballAcquired;
	DelayEvent targetAcquired;
	
	StateLatch< AutoTargetState > m_auto_target_state;

	DoubleProxy gyroVoltage;
public:
	RobotDemo(void):
		
		m_nosePointer( resources, 0.7, 1.0 ),
		m_position(resources, WHEEL_BASE),
		
		example(resources, m_nosePointer),
		vision(resources),
		kicker(resources),
		autonomousVision(resources, kicker, vision, m_nosePointer, m_position),
		
		ballAcquired(0.5),
		targetAcquired(0.5),
		
		m_auto_target_state(AT_NONE)
	{
		GetWatchdog().SetExpiration(0.1);

		// setup normal modes		
		modeControl.Add(&example);
		//modeControl.Add(&encoderMode);
		
		// setup autonomous modes
		autonomousModeControl.Add(&autonomousVision);
		
		m_position.Start();
		
		gyroVoltage = resources.webdma.CreateDoubleProxy("Main", "Voltage",
						DoubleProxyFlags().readonly()
		);
		
		printf("Starting kicker\n");
		kicker.Start();
	}

	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		
		autonomousModeControl.Enable();
		
		while(IsAutonomous() && IsEnabled())
		{
			autonomousModeControl.Run();
			// DO NOT TAKE THIS OUT
			Wait(0.002);
		}
		
		autonomousModeControl.Disable();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		
		// reset things each time
		m_auto_target_state = AT_ACQUIRE_BALL;
		m_motor_state = VB_NONE;
		
		modeControl.Enable();
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			gyroVoltage = resources.gyroChannel.GetVoltage();
			
			// kick the ball manually
			if( resources.stick.GetTrigger() ) kicker.Kick();
			
			// update the buttons each time around (inverted on purpose!)
			leftVision.Set( resources.stick.GetRawButton(VISION_RIGHT_BUTTON) );
			rightVision.Set( resources.stick.GetRawButton(VISION_LEFT_BUTTON) );
			eitherButton.Set( resources.stick.GetRawButton(VISION_EITHER_BUTTON) );
			autoKickButton.Set( DriverStation::GetInstance()->GetDigitalIn(3) );
			
			AutomatedKicking();
			
			// if the automated kicking isn't trying to acquire a target...
			if (m_auto_target_state != AT_ACQUIRE_TARGET)
			{
				ManualVisionTargeting();
			}
			
			if (m_motor_state == VB_NONE)
			{
				modeControl.Run();
			}
			else
			{
				resources.myRobot.ArcadeDrive(0.0, m_nosePointer.GetTurnRate( vision.GetVisionAngle() ) );
			}
						
			// DO NOT TAKE THIS OUT
			Wait(0.002);
		}
		
		// disable the motors when exiting user control
		resources.myRobot.ArcadeDrive(0.0, 0.0);
		
		modeControl.Disable();
	}
	
	/**
		\brief This function tries to do automated kicking using a simple state 
		machine. The way this should work is something like this:
		
			- Wait for ball detection event
			- Once ball is detected, attempt to focus on a target
			- Once target is acquired, kick ball away
			- Wait for kick to complete before trying again
	*/
	void AutomatedKicking()
	{
		bool has_ball = kicker.HasBall();
		
		// turning on/off the auto kick control button
		if (autoKickButton.TurnedOn())
		{
			m_auto_target_state = AT_ACQUIRE_BALL;
		}
		else if (autoKickButton.TurnedOff())
		{
			m_auto_target_state = AT_NONE;
		}
	
		
		// enter/leave acquire ball stage
		if (m_auto_target_state.EnteredState( AT_ACQUIRE_BALL ))
		{
			ballAcquired.Reset();
		}
		
		
		// enter/leave acquire target stage
		if (m_auto_target_state.EnteredState( AT_ACQUIRE_TARGET)) 
		{
			targetAcquired.Reset();
		}		
		else if (m_auto_target_state.LeftState( AT_ACQUIRE_TARGET ))
		{
			m_motor_state = VB_NONE;
		}
		
		
		// if the button is off, then just return and don't bother
		// -> note that this comes after the various state transitions
		//    that are done above this code. This is intentional.
		if (autoKickButton.Off())
			return;
		
		
		switch (m_auto_target_state)
		{
		case AT_ACQUIRE_BALL:

			// wait until the human gets the ball
			if( has_ball )
			{
				// for more than a split second
				//if (ballAcquired.DoEvent())
				//{
					// once they have it, take over and focus on a target
					// of course: if the user is currently trying to focus on a 
					// target, then prefer to let them continue on that path
					
					if (m_motor_state == VB_NONE)
					{
						m_motor_state = VB_EITHER;
						vision.PreferEither();
					}
						
					m_auto_target_state = AT_ACQUIRE_TARGET;
				//}
			}
			else
			{
				ballAcquired.Reset();
			}
			
			break;
			
		case AT_ACQUIRE_TARGET:
		
			// We have a ball, so attempt to focus on a target
			
			// .. unless we lost the ball, then give the human control again
			if (!has_ball)
				m_auto_target_state = AT_ACQUIRE_BALL;
						
			if(vision.IsRobotPointingAtTarget()) 
			{
				// wait until we have had the target in our sights for more than
				// a half second or so
				if (targetAcquired.DoEvent())
				{
					kicker.Kick();
					
					m_auto_target_state = AT_ACQUIRE_BALL;
					m_motor_state = VB_NONE;
				}
			}
			else
			{
				// reset the delay if we lose the target
				targetAcquired.Reset();
			}
			
			break;	
			
		case AT_NONE:
			break;
		}
	}
	
	/**
		Controls the state of vision targeting based on the buttons
		the user presses on the joystick
	*/
	void ManualVisionTargeting()
	{
		switch (m_motor_state)
		{
			case VB_LEFT:
				
				if (leftVision.TurnedOff())
					m_motor_state = VB_NONE;
				
				break;
			
			case VB_RIGHT:
			
				if (rightVision.TurnedOff())
					m_motor_state = VB_NONE;
				
				break;
			
			case VB_EITHER:

				if (eitherButton.TurnedOff())
					m_motor_state = VB_NONE;
			
				break;
			
			case VB_NONE:
			
				// only do the transition here if no other
				// button is selected, otherwise accidentally
				// hitting two buttons may disable the targeting
				
				if (eitherButton.TurnedOn())
				{
					vision.PreferEither();
					m_motor_state = VB_EITHER;
				}
				else if (leftVision.TurnedOn())
				{
					vision.PreferLeft();
					m_motor_state = VB_LEFT;
				}
				else if (rightVision.TurnedOn())
				{
					vision.PreferRight();
					m_motor_state = VB_RIGHT;
				}
			
				break;		
		}	
	}
	
};

START_ROBOT_CLASS(RobotDemo);

