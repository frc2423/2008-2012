

#ifndef AUTONOMOUS_RANDOM_DEMO
#define AUTONOMOUS_RANDOM_DEMO

class AutonomousRandomDemo : public KwarqsMovementControl {
public:

	AutonomousRandomDemo(KwarqsDriveController * driveController) :
		KwarqsMovementControl(driveController),
		m_start_time(0),
		m_tm(0)
	{}
	
	void OnEnable()
	{
		m_tm = m_start_time = GetTime();
		state = 0;
		srand(time(0));
	}

	void Move() 
	{
		double elapsed = GetTime() - m_start_time;
	
		if (elapsed > state * 1.5)
		{
			speed = (double)((rand() % 60) + 20)/100.0;
			angle = (double)(rand() % 360);
			rotation_goal = (double)(rand() % 90) + 135.0;
			
			state += 1;
		}
		
		rotation = m_nosePointer(rotation_goal);
			
		m_driveController->Move(speed, angle, rotation, false);
		
		if (GetTime() - m_tm > .25)
		{
			printf("%f: s: %.1f a: %1f r: %.1f\r", elapsed, speed, angle, rotation);
			m_tm = GetTime();
		}
	}
	
	/// This returns the name of the class (useful for generic logging)
	const char * Name() { return "AutonomousRandomDemo"; }
	
private:

	KwarqsNosePointer m_nosePointer;
	double m_start_time, m_tm;
	double speed, angle, rotation, rotation_goal;
	
	int state;
	
};

#endif
