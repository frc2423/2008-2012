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

	enum { VB_LEFT, VB_RIGHT, VB_EITHER, VB_NONE } m_motor_state;
	enum AutoTargetState { AT_NONE, AT_ACQUIRE_BALL, AT_ACQUIRE_TARGET };

	StackEnable unused;
	
	RobotResources resources;
	ExampleMode example;
	Vision vision;
	CompassMode compass;
	Kicker kicker;
	AutonomousVisionMode autonomousVision;
	EncoderMode encoderMode;
	Mode mode;
	
	TimedLatch eitherButton;
	TimedLatch leftVision;
	TimedLatch rightVision;
	
	TimedLatch autoKickButton;
	
	DelayEvent kickerPossession;
	DelayEvent autoKickDelay;
	
	StateLatch< AutoTargetState > m_auto_target_state;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		kicker(resources),
		autonomousVision(resources, kicker, vision, 1),
		encoderMode(resources),
		mode(&autonomousVision),
		
		kickerPossession(0.5),
		autoKickDelay(0.5),
		
		m_auto_target_state(AT_NONE)
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
		

		
		// reset things each time
		m_auto_target_state = AT_ACQUIRE_BALL;
		m_motor_state = VB_NONE;
		
		
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			// kick the ball manually
			if( resources.stick.GetTrigger() ) kicker.Kick();
			
			// update the buttons each time around
			leftVision.Set( resources.stick.GetRawButton(VISION_LEFT_BUTTON) );
			rightVision.Set( resources.stick.GetRawButton(VISION_RIGHT_BUTTON) );
			eitherButton.Set( resources.stick.GetRawButton(VISION_EITHER_BUTTON) );
			autoKickButton.Set( DriverStation::GetInstance()->GetDigitalIn(3) );
			
			AutomatedKicking();
			
			if (m_auto_target_state == AT_ACQUIRE_BALL)
			{
				ManualVisionTargeting();
				
				if (m_motor_state == VB_NONE)
				{
					// normal mode stuff here
					vision.DisableMotorControl();
					mode.run();
				}
			}
			
			// DO NOT TAKE THIS OUT
			Wait(0.005);
		}
		
		// disable the vision controlling if we exit operator control
		vision.DisableMotorControl();
	}
	
	
	void AutomatedKicking()
	{
		bool has_ball = kicker.HasBall();
		
		if (autoKickButton.TurnedOn() || !has_ball)
		{
			m_auto_target_state.Set( AT_ACQUIRE_BALL );
		}
		else if (autoKickButton.TurnedOff())
		{
			m_auto_target_state.Set( AT_NONE );
		}
	
		if (autoKickButton.Off())
			return;
		
		if (m_auto_target_state.EnteredState( AT_ACQUIRE_BALL ))
		{
			kickerPossession.Reset();
		}
		
		if (m_auto_target_state.EnteredState( AT_ACQUIRE_TARGET)) 
		{
			autoKickDelay.Reset();
		}
		
		else if (m_auto_target_state.LeftState( AT_ACQUIRE_TARGET ))
		{
			vision.DisableMotorControl();
			m_motor_state = VB_NONE;
		}
		
		
		
		switch (m_auto_target_state.State())
		{
		case AT_ACQUIRE_BALL:

			// wait until the human gets the ball
			if( has_ball )
			{
				if (kickerPossession.DoEvent())
				{
					vision.PreferEither();
					m_auto_target_state.Set( AT_ACQUIRE_TARGET );
					autoKickDelay.Reset();
				}
			}
			else
			{
				kickerPossession.DoEvent();
			}
			
			break;
			
		case AT_ACQUIRE_TARGET:
						
			if(vision.IsRobotPointingAtTarget()) 
			{
				// wait until some period of time elapses before kicking
				if (autoKickDelay.DoEvent())
				{
					// reset the manual targeting state too
					kicker.Kick();
					m_auto_target_state.Set( AT_ACQUIRE_BALL );
				}
			}
			else
			{
				// reset the delay if we lose the target
				autoKickDelay.Reset();
			}
			
			break;	
			
		case AT_NONE:
			break;
		}
	}
	
	
	void ManualVisionTargeting()
	{
		switch (m_motor_state)
		{
			case VB_LEFT:
				
				if (leftVision.TurnedOff())
				{
					vision.DisableMotorControl();
					m_motor_state = VB_NONE;
				}
				
				break;
			
			case VB_RIGHT:
			
				if (rightVision.TurnedOff())
				{
					vision.DisableMotorControl();
					m_motor_state = VB_NONE;
				}
				
				break;
			
			case VB_EITHER:

				if (eitherButton.TurnedOff())
				{
					vision.DisableMotorControl();
					m_motor_state = VB_NONE;
				}
			
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

