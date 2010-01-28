
#include "CoordinateSystem.h"
#include <cmath>
#include <limits>

#define M_PI 3.141592653589793238462643

#undef min
#undef max


CoordinateSystem::CoordinateSystem(	
		Encoder* leftencoder, 
		Encoder* rightencoder, 
		WebDMA* webdma
):
	m_Angle(0),
	m_wheelBase(0),
	m_notifier(CoordinateSystem::TimerFn, this),
	m_leftEncoder(leftencoder),
	m_rightEncoder(rightencoder)
{
	m_leftMeters = webdma->CreateDoubleProxy("Coordinate System", "left meters",
		DoubleProxyFlags()
			.default_value(0)
			.minval(std::numeric_limits<double>::min())
			.maxval(std::numeric_limits<double>::max())
			.step(1)
			.readonly()
	);
	
	m_rightMeters = webdma->CreateDoubleProxy("Coordinate System", "right meters",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	// create a mutex to allow us to synchronize access to variables
	// since we're going to be running in multiple threads
	m_mutex = semMCreate(0);
}

/*
 * 	wheel_radius - Radius of the wheel we are using (in meters)
 *  wheel_base	 - Distance between the wheels (in meters)
 */
void CoordinateSystem::SetWheelInformation (double wheel_radius, double wheel_base)
{
	m_wheelBase = wheel_base;
	
	m_leftEncoder->SetDistancePerPulse( 1440.0 / (2 * M_PI * wheel_radius) );
	m_rightEncoder->SetDistancePerPulse( 1440.0 / (2 * M_PI * wheel_radius) );
}

void CoordinateSystem::Start()
{
	m_notifier.StartPeriodic(.005);
}

// callback function for the Notifier 
void CoordinateSystem::TimerFn(void * param)
{ 
	((CoordinateSystem*)param)->CalculationTimerFn(); 
}

// this is called every 50ms and performs calculations
void CoordinateSystem::CalculationTimerFn ()
{
	// make sure that nobody else can access our variables
	Synchronized lock(m_mutex);
	
	
	// Store encoder values so we can use them next time.
	m_leftMeters = m_leftEncoder->GetDistance();
	m_rightMeters = m_rightEncoder->GetDistance();
}
