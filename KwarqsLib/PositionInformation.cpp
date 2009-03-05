/**
	\file 		PositionInformation.cpp
	\author 	Dustin Spicuzza (adapted from Rob McGurrin's C code),
				last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>
#include "math.h"

#include "Deleter.h"
#include "PositionInformation.h"
#include "KwarqsConstants.h"

// static variables
PositionInformation * PositionInformation::m_instance = NULL;
Deleter<PositionInformation> PositionInformation::deleter(&PositionInformation::m_instance);

// constructor
PositionInformation::PositionInformation() :
	m_notifier(NULL),
	m_gyro(GYRO_SLOT, GYRO_CHANNEL),
	
	m_accelerometerXIn(ACCELEROMETER_X_SLOT, ACCELEROMETER_X_INPUT),
	m_accelerometerYIn(ACCELEROMETER_Y_SLOT, ACCELEROMETER_Y_INPUT),
	
	m_field1(FIELD_SW1_SLOT, FIELD_SW2_CHANNEL),
	m_field2(FIELD_SW1_SLOT, FIELD_SW2_CHANNEL),
	
	m_gyro_offset(0.0),
	m_field_offset(0.0),

	xBias(0.0), yBias(0.0)
{
	m_gyro.SetSensitivity(0.007F);
	
	// create the mutex
	m_mutex = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	
	// specify the source
	countXhi.SetUpSource(m_accelerometerXIn);
	countXlow.SetUpSource(m_accelerometerXIn);
	countYhi.SetUpSource(m_accelerometerYIn);
	countYlow.SetUpSource(m_accelerometerYIn);
	
	// setup the counters
	countXhi.SetSemiPeriodMode(true);
	countXlow.SetSemiPeriodMode(false);
	countYhi.SetSemiPeriodMode(true);
	countYlow.SetSemiPeriodMode(false);
	
	countXhi.Start();
	countXlow.Start();
	countYhi.Start();
	countYlow.Start();
	
	// get the acceleration bias
	AverageWindowFilter<double, 20> fx, fy;
	double ax = 0, ay = 0;
	
	for (int i = 0; i < 20; i++)
	{
		Wait(0.05);
		GetAccel(ax, ay);
		fx.AddPoint(ax);
		fy.AddPoint(ay);
	}
	
	xBias = fx.GetAverage();
	yBias = fy.GetAverage();
	
	// start this up at least once
	CalculatePositionInformation();

	// start the calculations
	m_notifier = new Notifier(PositionInformation::PeriodicFunction, this);
	m_notifier->StartPeriodic(PINFO_CALCULATION_PERIOD);
}

// destructor
PositionInformation::~PositionInformation()
{
	if (m_notifier)
		delete m_notifier;
}



/// called every 50ms
void PositionInformation::CalculatePositionInformation()
{
	Synchronized sync(m_mutex);
	
	double x, y;
	GetAccel(x, y);

	avgAx.AddPoint(x - xBias);
	avgAy.AddPoint(y - yBias);
	
	// do the field offset here
	bool f1 = m_field1.Get() ? true : false;
	bool f2 = m_field2.Get() ? true : false;
	
	int pos = ((int)f1 << 1) | (int)f2; 
	
	// the field offset is added to the gyro angle
	switch (pos)
	{
		case 0:
			m_field_offset = 0;
			break;
			
		case 1:
			m_field_offset = 225;
			break;
			
		case 2:
			m_field_offset = 90;
			break;
			
		case 3:
			m_field_offset = 315;
			break;
	}
}





/// returns the acceleration of the bot in m/s^2
void PositionInformation::GetAcceleration(double &x, double &y)
{
	Synchronized sync(m_mutex);
	
	x = avgAx.GetAverage();
	y = avgAy.GetAverage();
}



/// returns the angle the robot is currently facing relative to the field
double PositionInformation::GetFieldAngle()
{
	Synchronized sync(m_mutex);
	return (m_gyro.GetAngle() - m_gyro_offset) + m_field_offset;
}

/// returns the normalized angle (0-360), relative to the field
double PositionInformation::GetNormalizedFieldAngle()
{
	double angle = fmod(GetFieldAngle(), 360.0);
	if (angle < 0)
		angle += 360;
	return angle;
}

/// returns the raw angle from the gyro
double PositionInformation::GetRawAngle()
{
	return m_gyro.GetAngle();
}

/// returns the raw angle from the gyro, but normalized to 0-360
double PositionInformation::GetNormalizedRawAngle()
{
	double angle = fmod(m_gyro.GetAngle(), 360.0F);
	if (angle < 0)
		angle += 360;
	return angle;
}

double PositionInformation::GetFieldOffset()
{
	Synchronized sync(m_mutex);
	return m_field_offset;
}

void PositionInformation::ResetHeading()
{
	Synchronized sync(m_mutex);
	m_gyro_offset = m_gyro.GetAngle() + m_field_offset;
}


/// returns a normalized angle, relative to the robot
double PositionInformation::TranslateFieldToRobotAngle(double angle)
{
	Synchronized sync(m_mutex);

	angle = ((m_gyro.GetAngle() - m_gyro_offset) + m_field_offset) - angle;

	angle = fmod(angle, 360.0);
	if (angle < 0)
		angle += 360;
	
	return angle;
}


/// should only be called from mutex-protected functions
void PositionInformation::GetAccel(double &ax, double &ay)
{
	double axH = countXhi.GetPeriod();
	double axL = countXlow.GetPeriod();
	double ayH = countYhi.GetPeriod();
	double ayL = countYlow.GetPeriod();
					
	// convert to m/s^2 -- 50% duty cycle is 0g
	ax = (((axH / (axH + axL)) - .5) * 8.0) * 9.81;
	ay = (((ayH / (ayH + ayL)) - .5) * 8.0) * 9.81;
	
	// safety check here for infinity (if the timer is stalled)
	if (ax > 1000000000 || ax < -1000000000)
		ax = 0;
	if (ay > 1000000000 || ay < -1000000000)
		ay = 0;
}


