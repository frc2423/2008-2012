

#ifndef AUTONOMOUS_DEMO
#define AUTONOMOUS_DEMO

class AutonomousDemo : public KwarqsMovementControl {
public:

	AutonomousDemo(KwarqsDriveController * driveController) :
		KwarqsMovementControl(driveController),
		m_start_time(0),
		m_tm(0)
	{}
	
	void OnEnable()
	{
		m_tm = m_start_time = GetTime();
	}

	void Move() 
	{
		double elapsed = GetTime() - m_start_time;
	
		double speed = 0, angle = 0, rotation = 0;
		bool stop = false;
	
		// rotate to 180 in two seconds
		if (elapsed < 3)
		{
			speed = 1;
			angle = 0;
			rotation = m_nosePointer.GetRotation(180);
		}
		else if (elapsed < 6)
		{
			speed = 1;
			angle = 180;
			rotation = m_nosePointer.GetRotation(180);
		}
		else if (elapsed < 9)
		{
			speed = .5;
			angle = 270;
			rotation = m_nosePointer.GetRotation(90);
		}
		
		else if (elapsed < 12)
		{
			speed = 0.2;
			angle = 0;
			rotation = 1;
		}
		else
			stop = true;
			
		m_driveController->Move(speed, angle, rotation, stop);
		
		if (GetTime() - m_tm > .25)
		{
			printf("%f: s: %.1f a: %1f r: %.1f\r", elapsed, speed, angle, rotation);
			m_tm = GetTime();
		}
	}
	
	/// This returns the name of the class (useful for generic logging)
	const char * Name() { return "AutonomousDemo"; }
	
private:

	KwarqsNosePointer m_nosePointer;
	double m_start_time, m_tm;
	
};

#endif
