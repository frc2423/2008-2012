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

#include "../WebInterface/WebInterface.h"

#define NP_SQRT_RANGE 225.0
#define NP_ERROR_ANGLE 1.0

/**
	\class KwarqsNosePointer
	\brief This calculates a rotation to get your nose pointing in a 
	particular direction relative to the field. Uses PID control
*/
class KwarqsNosePointer {
public:

	KwarqsNosePointer(const char * groupname = NULL) : 
		m_sampleEvent(0.05),
		m_result(0),
		m_position(PositionInformation::GetInstance()),
		m_has_proxy(groupname ? true : false)
	{
		if (m_has_proxy)
		{
			m_sqrt_range = WebInterface::CreateDoubleProxy(groupname, "SQRT Range", 
					DoubleProxyFlags().minval(0).maxval(360).default_value(NP_SQRT_RANGE).step(1));
			m_error_angle = WebInterface::CreateDoubleProxy(groupname, "Error Angle", 
					DoubleProxyFlags().minval(0).maxval(360).default_value(NP_ERROR_ANGLE).step(1));
			
			m_setpoint = WebInterface::CreateDoubleProxy(groupname, "Point angle", 
					DoubleProxyFlags().readonly().default_value(0));
			m_error = WebInterface::CreateDoubleProxy(groupname, "Current Error", 
					DoubleProxyFlags().readonly().default_value(0));
		}
	}
	
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
			
			if (m_has_proxy)
				m_error = error;
			
			// find the shortest rotation distance
			if (fabs(error) > 180)
			{
				if (error > 0)
					error -= 360;
				else
					error += 360;
			}
			
			double error_angle = m_has_proxy ? m_error_angle : NP_ERROR_ANGLE;
			
			// stop rotating depending on the error
			if (fabs(error) < error_angle)
				m_result = 0;
			else
			{
				// scale the rotation parameter according to how close we currently are		
				
				// linear:
				//m_result = (error * .5) / 180;
				
				double sqrt_range = m_has_proxy ? m_sqrt_range : NP_SQRT_RANGE;
				
				// sqrt:
				if (error < 0)
					m_result = ((sqrt(fabs(error))) / sqrt(sqrt_range))*-1.0;
				else
					m_result = (sqrt(error)) / sqrt(sqrt_range);
			}
		
			// print something useful out
			DriverStationLCD::GetInstance()->PrintfLine(DriverStationLCD::kUser_Line6, "@: %5.1f To: %5.1f", angle, setpoint);
		}
		
		if (m_has_proxy)
			m_setpoint = setpoint;
			
		// return the calculation
		return m_result;
	}
	
private:

	DelayEvent m_sampleEvent;
	double m_result;
	
	PositionInformation * m_position;
	
	DoubleProxy m_sqrt_range;
	DoubleProxy m_setpoint;
	DoubleProxy m_error;
	DoubleProxy m_error_angle;
	
	bool m_has_proxy;
};

#endif

