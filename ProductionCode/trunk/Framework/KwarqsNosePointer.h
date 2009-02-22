
#ifndef FAKE_PID_CONTROLLER_H
#define FAKE_PID_CONTROLLER_H

#include "PositionInformation.h"
#include "math.h"

/**
	\class KwarqsNosePointer
	\brief This calculates a rotation to get your nose pointing in a 
	particular direction relative to the field
*/
class KwarqsNosePointer {
public:

	KwarqsNosePointer() : 
		last_sample_time(0),
		m_result(0),
		m_position(PositionInformation::GetInstance())
	{}
	
	
	double GetRotation(double setpoint)
	{
		if (GetTime() - last_sample_time > 0.05)
		{
			// normalize the direction we want to go to
			setpoint = fmod(setpoint, 360.0);
			if (setpoint < 0) setpoint += 360;
		
			// get the direction we're currently facing
			double angle = m_position->GetNormalizedAngle()*-1;
			if (angle <0) angle += 360;
			
			
			
			// get the difference between the angles
			double error = setpoint - angle;
			
			// find the shortest rotation distance
			if (fabs(error) > 180)
			{
				if (error > 0)
					error -= 360;
				else
					error += 360;
			}
			
			// any error under 1 degree is ok by me, stop rotating
			if (fabs(error) < 1.0)
				m_result = 0;
			else
			{
				// scale the rotation parameter according to how close we currently are		
				
				// linear:
				//m_result = (error * .5) / 180;
				
				// sqrt:
				if (error < 0)
					m_result = ((sqrt(fabs(error))) / sqrt(190.0))*-1.0;
				else
					m_result = (sqrt(error)) / sqrt(190.0);
			}
			
		
			last_sample_time = GetTime();
		}
			
		// return the calculation
		return m_result;
	}
	
private:

	double last_sample_time;
	double m_result;
	
	PositionInformation * m_position;
};

#endif

