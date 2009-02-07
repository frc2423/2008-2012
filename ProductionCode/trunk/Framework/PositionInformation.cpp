/**
	\file 		PositionInformation.cpp
	\author 	Dustin Spicuzza (adapted from Rob McGurrin's C code),
				last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>
#include <math.h>

#include "Deleter.h"
#include "PositionInformation.h"

// static variables
PositionInformation * PositionInformation::m_instance = NULL;
Deleter<PositionInformation> PositionInformation::deleter(&PositionInformation::m_instance);

// constructor
PositionInformation::PositionInformation() :
	m_gyro(GYRO_CHANNEL),
	m_accelerometerX(ACCELEROMETER_X_CHANNEL),
	m_accelerometerY(ACCELEROMETER_Y_CHANNEL),
	
	m_gyro_offset(0.0),
	
	buffIndex(0),

	xUpdate(0.0), yUpdate(0.0),

	xVelocity(0.0), yVelocity(0.0),
	xPos(0.0), yPos(0.0),

	xBias(0.0), yBias(0.0)
{
	// create the mutex
	m_mutex = semBCreate(0, SEM_FULL);

	// clear the array
	for (int i = 0; i < ACCEL_HIST_LEN; i++)
		xBuff[i] = yBuff[i] = 0.0;
	/*
	// automatic calibration
	for (int i = 0; i < ACCEL_HIST_LEN; i++)
	{
		CalculatePositionInformation();
		Wait(PINFO_CALCULATION_PERIOD);
	}
	
	EstimateAccelerationBias();
	*/
	
	// todo: need to get our alliance and position, and calculate the offset
	// of the gyro to the driver for absolute numbers

	// start the calculations
	m_notifier = new Notifier(PositionInformation::PeriodicFunction, this);
	m_notifier->StartPeriodic(PINFO_CALCULATION_PERIOD);	
}

// destructor
PositionInformation::~PositionInformation()
{
	delete m_notifier;
}


/// returns the position of the bot on the field in X/Y coordinates
void PositionInformation::GetPosition(double &x, double &y)
{
	Synchronized sync(m_mutex);
	x = xPos;
	y = yPos;
}

/// returns the angle the robot is moving, relative to the robot
double PositionInformation::GetHeading()
{
	Synchronized sync(m_mutex);
	// todo: should we use velocity or acceleration to get this?
	return 0;
}

/// returns the angle the robot is currently facing relative to the field
double PositionInformation::GetAngle()
{
	return m_gyro.GetAngle() - m_gyro_offset;
}

/// returns the acceration of the robot, relative to the robot
/// Note: uses the parameters to return the values by reference
void PositionInformation::GetAcceleration(double &x, double &y)
{
	Synchronized sync(m_mutex);
	x = xUpdate;
	y = yUpdate;
}

/// returns the velocity of the robot, relative to the robot
/// Note: Uses the parameters to return the values by reference
void PositionInformation::GetVelocity(double &x, double &y)
{
	Synchronized sync(m_mutex);
	x = xVelocity;
	y = yVelocity;
}
	

	

// called every N milliseconds to calculate the information needed
void PositionInformation::CalculatePositionInformation()
{
  Synchronized sync(m_mutex);
  
  double x = m_accelerometerX.GetAcceleration() * 9.81;
  double y = m_accelerometerY.GetAcceleration() * 9.81;

  double alpha = FILT_COEFF;
  double mag;
  
  //mexPrintf("X: %f Y: %f\n",x,y);
  
  /* store the acclerations for estimating the bias */
  yBuff[buffIndex] = y;
  xBuff[buffIndex] = x;
  buffIndex++;
  if( buffIndex > ACCEL_HIST_LEN ) 
      buffIndex = 0;

  /* filter the accelerations and subtract the means */
  xUpdate = alpha*( x - xBias ) + (1-alpha)*xUpdate;
  yUpdate = alpha*( y - yBias ) + (1-alpha)*yUpdate;
    
  /* compute the magnitude of the acceleration to check if we are moving */
  mag = sqrt(xUpdate*xUpdate + yUpdate*yUpdate);
  
  if( mag > MOTION_THRESH ) 
  {
      
    /* update velocity and position */  
    xVelocity += xUpdate*PINFO_CALCULATION_PERIOD;
    xPos += xVelocity*PINFO_CALCULATION_PERIOD;
    
    yVelocity += yUpdate*PINFO_CALCULATION_PERIOD;
    yPos += yVelocity*PINFO_CALCULATION_PERIOD;
  }
  else
  {
    xVelocity = 0;
    yVelocity = 0;
  }
}

// todo: this is used for calibration only, right? Or should it
// be called every once in awhile?
void PositionInformation::EstimateAccelerationBias()
{
  Synchronized sync(m_mutex);
  int ind;
  double xSum = 0, ySum = 0;
  
  for( ind = 0; ind < ACCEL_HIST_LEN; ind++ )
  {
    xSum += xBuff[ind];
    ySum += yBuff[ind];
      
  }
  
  xBias = xSum/ACCEL_HIST_LEN;
  yBias = ySum/ACCEL_HIST_LEN;  
}

void PositionInformation::GetAccelerationBias( double &x, double &y)
{
	x = xBias;
	y = yBias;
}

void PositionInformation::SetAccelerationBias( double x, double y)
{
	xBias = x;
	yBias = y;
}





