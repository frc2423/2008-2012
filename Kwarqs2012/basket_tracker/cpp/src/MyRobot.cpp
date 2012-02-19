
#include "WPILib.h"
#include "../BasketTracker.h"


class RobotDemo : public SimpleRobot {
	
	BasketTracker m_basketTracker;

public:
	RobotDemo(void) 
	{
		m_basketTracker.SetVerbose(true);
	}

	void OperatorControl(void) {
	}
};

START_ROBOT_CLASS(RobotDemo)
;
