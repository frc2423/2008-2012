/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef GYRO_H_
#define GYRO_H_

#include "SensorBase.h"
#include "PIDSource.h"
#include <Simulator/Simulator.h>


/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 * The Gyro class tracks the robots heading based on the starting position. As the robot
 * rotates the new heading is computed by integrating the rate of rotation returned
 * by the sensor. When the class is instantiated, it does a short calibration routine
 * where it samples the gyro while at rest to determine the default offset. This is
 * subtracted from each sample to determine the heading.
 */
class Gyro : public SensorBase, public PIDSource
{
public:
	explicit Gyro(UINT32 channel)
	{
		m_ok = false;
		Simulator::AddGyro(this, channel);
	}
	virtual ~Gyro()
	{
		m_ok = true;
		Simulator::DeleteGyro(this);
	}
	
	float GetAngle() { 
		assert(m_ok); 
		return m_angle; 
	}
	
	void SetSensitivity(float voltsPerDegreePerSecond){ 
		m_ok = true; 
	}
	
	void Reset() { 
		m_angle = 0; 
	}
	
	double PIDGet() { 
		return GetAngle(); 
	}
	
	float m_angle;
	
private:
	bool m_ok;
};
#endif
