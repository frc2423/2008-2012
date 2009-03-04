/**
	\file 		PositionInformation.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_POSITION_INFORMATION_H
#define KWARQS_POSITION_INFORMATION_H 

#include "KwarqsConstants.h"
#include "Deleter.h"

// defined parameters
#define PINFO_CALCULATION_PERIOD 	0.05		// period calculation is run


/**
	\class PositionInformation
	\brief Provides methods to get the position of the robot
	
	There will be a single instance of this class, and pointers of this 
	should be passed in to other classes that need position information. 
	
	Call GetInstance() to get a pointer to the instance of this class. 
	
*/
class PositionInformation {
public:
	
	/// used to get the instance of this class, should be called ASAP
	static PositionInformation * GetInstance()
	{
		if (!PositionInformation::m_instance)
			PositionInformation::m_instance = new PositionInformation();
		return PositionInformation::m_instance;
	}

	~PositionInformation();
	
	/// returns the angle (in degrees) the robot is facing relative to the field
	double GetFieldAngle();
	
	/// returns the angle (in degrees) the robot is facing relative to the 
	/// field, in the range of 0 to 360
	double GetNormalizedFieldAngle();
	
	/// returns the raw gyro value
	double GetRawAngle();
	
	/// returns the raw gyro value, normalized to 0-360
	double GetNormalizedRawAngle();
	
	/// returns the acceration of the robot, relative to the robot
	/// Note: uses the parameters to return the values by reference
	void GetAcceleration(double &x, double &y);
	
	
	void ResetHeading();
	
private:

	// this is private, since this is a singleton class
	PositionInformation();

	static PositionInformation * m_instance;
	static Deleter<PositionInformation> deleter;
	
	
	// needed for the periodic calculation
	static void PeriodicFunction(void * arg)
	{
		((PositionInformation*)arg)->CalculatePositionInformation();
	}
	
	// called every N milliseconds to calculate the information needed
	void CalculatePositionInformation();
	
	Notifier*		m_notifier;
	
	// Objects
	Gyro 			m_gyro;
	
	// heading information
	
	// what is our starting position?
	double			m_gyro_offset;
	
	// needed calculation variables for positioning
	// NOTE: Anytime these are accessed, use the mutex to ensure
	// proper access

	double 			xBias;
	double 			yBias;
	
	AverageWindowFilter<double, 20> avgAx, avgAy;
	
	// mutex used to provide access
	SEM_ID			m_mutex;
};

#endif
