

#ifndef AUTONOMOUS_DEMO
#define AUTONOMOUS_DEMO

#include "../KwarqsLib/KwarqsNosePointer.h"
#include "../KwarqsLib/KwarqsMovementControl.h"

class AutonomousDemo : public KwarqsMovementControl {
public:

	AutonomousDemo(KwarqsDriveController * driveController) :
		KwarqsMovementControl(driveController),
		m_start_time(0),
		m_speedEvent(0.025),
		m_lastSpeed(0),
		m_info(PositionInformation::GetInstance())
	{}	
	
	/*
	
		Autonomous state machine information:
		
		We assume that our state is based on time for simplicity,
		and so each one should just modify the incoming variables
		according to time.	
		
		If the variables are not modified, then the robot will attempt
		to spin continuously.
		
		Speed:
		
		Set it to whatever you want, it will get a fixed scaling
		applied to it. 
		
		Angle:
		
		When you set angle, set it relative to the driver's view, it
		will get translated to the correct angle relative to the robot
		automagically. 
		
		Rotation:
		
		When setting rotation to something other than zero, use the
		m_nosePointer class, it will return an appropriate rotation
		to get the robot pointing in the correct direction, relative
		to the field.
	
	*/
	
	
	// this should never run
	void Move0(double &speed, double &angle, double &rotation, double elapsed)
	{
	
		/*
		if (elapsed < 1)
		{
			speed = .5;
			angle = 45;
			rotation = 0;
		}
		else if (elapsed < 3)
		{
			speed = 1;
			angle = 45;
			rotation = m_nosePointer.GetRotation(180);
		}
		else if (elapsed < 6)
		{
			speed = .5;
			angle = 45;
			rotation = m_nosePointer.GetRotation(180);
		}
		*/
		
		speed = .5;
		angle = 45;
		rotation = 0;
		
		
	}
	
	// starting from the top left corner
	void Move1(double &speed, double &angle, double &rotation, double elapsed)
	{
		if (elapsed < 1)
		{
			speed = .5;
			angle = 245;
			rotation = m_nosePointer.GetRotation(315);
		}
		else if (elapsed < 3)
		{
			speed = 1;
			angle = 270;
			rotation = m_nosePointer.GetRotation(0);
		}
		else if (elapsed < 6)
		{
			speed = 1;
			angle = 270;
			rotation = m_nosePointer.GetRotation(90);
		}
		else
		{
			speed = 0;
			angle = 0;
			rotation = -.75;
		}
		
		// spin from then on
	}
	
	// starting from the middle, facing left
	void Move2(double &speed, double &angle, double &rotation, double elapsed)
	{
		if (elapsed < 1.5)
		{
			speed = .5;
			angle = 45;
			rotation = m_nosePointer.GetRotation(45);
		}
		else if (elapsed < 3)
		{
			speed = 1;
			angle = 0;
			rotation = m_nosePointer.GetRotation(0);
		}
		else if (elapsed < 6)
		{
			speed = 1;
			angle = -45;
			rotation = m_nosePointer.GetRotation(180);
		}
		else
		{
			speed = 0;
			angle = 0;
			rotation = .75;
		}
	}
	
	// starting from the bottom left corner
	void Move3(double &speed, double &angle, double &rotation, double elapsed)
	{
		if (elapsed < 1)
		{
			speed = .5;
			angle = 300;
			rotation = m_nosePointer.GetRotation(270);
		}
		else if (elapsed < 3)
		{
			speed = 1;
			angle = 270;
			rotation = m_nosePointer.GetRotation(180);
		}
		else if (elapsed < 6)
		{
			speed = 1;
			angle = 270;
			rotation = m_nosePointer.GetRotation(90);
		}
		else
		{
			speed = 0;
			angle = 0;
			rotation = -.75;
		}
	}
	
	
	/******************************************************
	 *
	 * Should not need to modify anything below this!
	 *
	 ******************************************************/
	
	void OnEnable()
	{
		m_start_time = GetTime();
		m_lastSpeed = 0;
	}

	void Move() 
	{
		double elapsed = GetTime() - m_start_time;
	
		// by default, just spin continuously
		double speed = 0.1, angle = 0, rotation = 1;
		
		switch (m_info->GetFieldPosition())
		{

				
			case 1:
				Move1(speed, angle, rotation, elapsed);
				break;
				
			case 2:
				Move2(speed, angle, rotation, elapsed);
				break;
				
			case 3:
				Move3(speed, angle, rotation, elapsed);
				break;
				
			default:
				Move0(speed, angle, rotation, elapsed);
				break;
		}
		
		// translate the heading from field to robot space
		angle = m_info->TranslateFieldToRobotAngle(angle);
		
		// let the speed semi-gradually ramp up
		//if (m_speedEvent.DoEvent())
		//	m_lastSpeed = m_lastSpeed + (speed - m_lastSpeed) * .1;
			
		// send it on
		m_driveController->Move(speed, angle, rotation, false);
	}
	
	
	/// This returns the name of the class (useful for generic logging)
	const char * Name() { return "AutonomousDemo"; }
	
private:

	KwarqsNosePointer m_nosePointer;
	double m_start_time, m_tm;
	
	DelayEvent m_speedEvent;
	double m_lastSpeed;
	
	PositionInformation * m_info;
	
};

#endif
