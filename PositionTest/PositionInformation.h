/**
	\file 		PositionInformation.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-01-25 03:23:53 -0500 (Sun, 25 Jan 2009) $
	\version 	$Rev: 46 $
*/

#ifndef KWARQS_POSITION_INFORMATION_H
#define KWARQS_POSITION_INFORMATION_H 

#include "KwarqsDevices.h"

// defined parameters
#define PINFO_CALCULATION_PERIOD 	0.05		// period calculation is run
#define ACCEL_HIST_LEN 				200 		// 10s of data
#define FILT_COEFF 					0.05		// filter coefficient
#define MOTION_THRESH 				0.4			// acceleration change needed to be considered 'motion'


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

	/// returns the position of the bot on the field in X/Y coordinates
	void GetPosition(double &x, double &y);
	
	/// returns the angle the robot is moving, relative to the robot
	double GetRelativeHeading();
	
	/// returns the angle the robot is currently facing, mod 360
	double GetAbsoluteHeading();
	
	/// returns the acceration of the robot, relative to the robot
	/// Note: uses the parameters to return the values by reference
	void GetAcceleration(double &x, double &y);
	
	/// returns the velocity of the robot, relative to the robot
	/// Note: Uses the parameters to return the values by reference
	void GetVelocity(double &x, double &y);
	
	
	// calibration stuff below:
	
	/// set the bias of the acceleration
	void SetAccelerationBias( double x, double y);
	
	/// estimate the bias of the acceleration
	void EstimateAccelerationBias( double *x, double *y );
	
private:

	// this is private, since this is a singleton class
	PositionInformation();

	static PositionInformation * m_instance;
	
	
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
	Accelerometer 	m_accelerometerX;
	Accelerometer 	m_accelerometerY;
	
	
	// heading information
	
	// what is our starting position?
	double			m_gyro_offset;
	
	// needed calculation variables for positioning
	// NOTE: Anytime these are accessed, use the mutex to ensure
	// proper access
	double 			yBuff[ACCEL_HIST_LEN];
	double 			xBuff[ACCEL_HIST_LEN];
	int 			buffIndex;

	double 			xUpdate;
	double 			yUpdate;

	double 			xVelocity;
	double 			yVelocity;
	double 			xPos;
	double 			yPos;

	double 			xBias;
	double 			yBias;
	
	// mutex used to provide access
	SEM_ID			m_mutex;
};

#endif
