/**
	\file 		KwarqsNosePointer.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_NOSEPOINTER_H
#define KWARQS_NOSEPOINTER_H

#include "DriverStationLCD.h"
#include "PositionInformation.h"
#include "math.h"
#include "DelayEvent.h"

/**
	\class KwarqsNosePointer
	\brief This calculates a rotation to get your nose pointing in a 
	particular direction relative to the field. Uses PID control
*/
class KwarqsNosePointer {
public:

	KwarqsNosePointer() : 
		m_sampleEvent(0.05),
		m_result(0),
		m_position(PositionInformation::GetInstance())
	{}
	
	/**
		Pass this an angle, and this will calculate a rotation parameter
		needed to point the robot in that direction. The angle is relative
		to the field. 
	*/
	double GetRotation(double setpoint)
	{
		if (m_sampleEvent.DoEvent())
		{
			// normalize the direction we want to go to
			setpoint = fmod(setpoint, 360.0);
			if (setpoint < 0) setpoint += 360;
			
		
			/// @todo this needs to be customizable
			// get the direction we're currently facing
			double angle = m_position->GetNormalizedFieldAngle()*-1;
			if (angle < 0) angle += 360;
			
			
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
		
			// print something useful out
			DriverStationLCD::GetInstance()->PrintfLine(DriverStationLCD::kUser_Line6, "@: %5.1f To: %5.1f", angle, setpoint);
		}
			
		// return the calculation
		return m_result;
	}
	
private:

	DelayEvent m_sampleEvent;
	double m_result;
	
	PositionInformation * m_position;
};

#endif

