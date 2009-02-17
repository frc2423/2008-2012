

#ifndef AUTONOMOUS_DEMO
#define AUTONOMOUS_DEMO

class AutonomousDemo : KwarqsMovementControl {
public:

	AutonomousDemo(KwarqsDriveController * driveController) :
		KwarqsMovementControl(driveController)
	{}
	
	void OnEnable()
	{
		m_time = GetTime();
	}

	void Move() 
	{
		double elapsed = GetTime() - m_time;
	
		double speed = 0, angle = 0, rotation = 0;
		bool stop = false;
	
		// rotate to 180 in two seconds
		if (elapsed < 2)
		{
			speed = 1;
			angle = 0;
			rotation = m_nosePointer.GetRotation(180);
		}
		else if (elapsed < 3)
		{
			// go forward one second
			speed = .5;
			angle = 45;
			rotation = m_nosePointer.GetRotation(180);
		}
		else if (elapsed < 5)
		{
			// go sideways another second
			speed = .5;
			angle = 270;
			rotation = m_nosePointer.GetRotation(90);
		}
		
		else if (elapsed < 7)
		{
			// spin for a few seconds
			speed = 0;
			angle = 0;
			rotation = .2;
		}
		else
			stop = true;
			
		m_driveController->Move(speed, angle, rotation, stop);
	}
	
	/// This returns the name of the class (useful for generic logging)
	const char * Name() { return "AutonomousDemo"; }
	
private:

	KwarqsNosePointer m_nosePointer;
	double m_time = 0;
	
	
};

#endif
