
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
	m_angle(0),
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
	
	m_xDistance = webdma->CreateDoubleProxy("Coordinate System", "X",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	m_yDistance = webdma->CreateDoubleProxy("Coordinate System", "Y",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	m_display_angle = webdma->CreateDoubleProxy("Coordinate System", "Angle",
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
	
	m_leftEncoder->SetDistancePerPulse( (2 * M_PI * wheel_radius) / 1440.0 );
	m_rightEncoder->SetDistancePerPulse( (2 * M_PI * wheel_radius) / 1440.0 );
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
	double prev_leftMeters = m_leftMeters;
	double prev_rightMeters = m_rightMeters;
	
	m_leftMeters = m_leftEncoder->GetDistance();
	m_rightMeters = - m_rightEncoder->GetDistance();
	
	double leftDistance = m_leftMeters - prev_leftMeters;
	double rightDistance = m_rightMeters - prev_rightMeters;
	
	double angle = (leftDistance - rightDistance) / (m_wheelBase / 2);
	m_angle += angle;
	
	m_display_angle = m_angle * (180 / M_PI);
	
	double x_Distance = leftDistance * cos(m_angle);
	m_xDistance = m_xDistance + x_Distance;
	
	double y_Distance = leftDistance * sin(m_angle);
	m_yDistance = m_yDistance + y_Distance;
}
