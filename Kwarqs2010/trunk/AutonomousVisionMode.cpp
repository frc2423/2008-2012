#include "AutonomousVisionMode.h"
#include "WPILib.h"
#include <math.h>

AutonomousVisionMode::AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision &vision, int balls):
	m_resources(resources),
	m_kicker(kicker),
	m_vision(vision),
	m_balls(balls),
{	

}

void AutonomousVisionMode::OnEnable()
{

}

void AutonomousVisionMode::Main()
{
	// STATIC VARIABLES ARE BAD
	static int case_Number = 1;
	static int balls_kicked = 0;
	
	switch(case_Number)
	{
	case 1:
		//against the wall, moving in a straight line, kicking
		m_resources.myRobot.Drive(-0.5, 0.0);
		// if (photogate.SeesBall()) case_Number = 2;
		break;
	case 2:
		//robot stops to kick ball
		m_resources.myRobot.Drive(0.0, 0.0);
		case_Number = 3;
		break;
	case 3:
		// robot aligns with ball
		m_vision.PreferEither();
		if (m_vision.IsRobotPointingAtTarget()) 
		{
			m_vision.DisableMotorControl();
			case_Number = 4;
		}
		break;
	case 4:
		// robot kicks ball
		m_kicker.Kick();
		balls_kicked++;
		break;
	case 5:
		// robot realigns with next ball
		double error = m_resources.gyro.GetAngle();
		error = error / 180.0;
		m_resources.myRobot.Drive(0.2, error);
		if (fabs(error) <= 1.0) case_Number = 6;
		break;
	case 6:
		//Go back to case 1 if there are more balls to kick
		if(balls_kicked < m_balls) case_Number = 1;
		else case_Number = 7;
		break;
	case 7:
		m_resources.myRobot.Drive(0.0, 0.0);
		break;
	}
}

void AutonomousVisionMode::OnDisable()
{
}

