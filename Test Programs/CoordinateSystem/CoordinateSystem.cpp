
#include "CoordinateSystem.h"
#include <cmath>

#define M_PI 3.141592653589793238462643

#include <limits>
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
void CoordinateSystem::SetWheelInformation (const double wheel_radius, const double wheel_base)
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
/*
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
	
	double radius, leftRadius, rightRadius;
	
	if (fabs(leftDistance) >= fabs(rightDistance))
	{
		radius = (leftDistance / ( d_err(leftDistance - rightDistance))) * m_wheelBase - m_wheelBase;
		leftRadius = m_wheelBase - fabs(radius);
		rightRadius = fabs(radius);
	}
	else
	{
		radius = (rightDistance / (d_err(rightDistance - leftDistance))) * m_wheelBase - m_wheelBase;
		leftRadius = fabs(radius);
		rightRadius = m_wheelBase - fabs(radius);
	}
	
	double angle = (leftDistance / d_err(leftRadius)) - (rightDistance / d_err(rightRadius));
	
	m_angle += angle;
	
	m_display_angle = m_angle * (180.0 / M_PI);
	
	double modAngle = fmod(m_angle, (2.0 * M_PI));
	
	double x_Distance = leftDistance * fabs(cos(m_angle));
	double y_Distance = leftDistance * fabs(sin(m_angle));
	
	if ((modAngle > M_PI) || (modAngle > -M_PI && modAngle < 0))
	{
		x_Distance *= -1.0;
	}
	
	if ((abs(modAngle) > (M_PI / 2.0)) && (fabs(modAngle) < (3.0 * M_PI / 2.0)))
	{
		y_Distance *= -1.0;
	}
	
	m_xDistance = m_xDistance + x_Distance;
	m_yDistance = m_yDistance + y_Distance;
}
*/
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
	

	
	
	double angle = (leftDistance - rightDistance) / (m_wheelBase / 2.0);
	
	m_angle += angle;
	
	m_display_angle = m_angle * (180.0 / M_PI);
	
	double modAngle = fmod(m_angle, (2.0 * M_PI));
	
	double x_Distance = leftDistance * fabs(cos(m_angle));
	double y_Distance = leftDistance * fabs(sin(m_angle));
	
	if ((modAngle > M_PI) || (modAngle > -M_PI && modAngle < 0))
	{
		x_Distance *= -1.0;
	}
	
	if ((abs(modAngle) > (M_PI / 2.0)) && (fabs(modAngle) < (3.0 * M_PI / 2.0)))
	{
		y_Distance *= -1.0;
	}
	
	m_xDistance = m_xDistance + x_Distance;
	m_yDistance = m_yDistance + y_Distance;
}

// returns x and y relative to robot starting posititon, and angle in degrees, by reference
void CoordinateSystem::getData(double &x, double &y, double &angle)
{
	// make sure that nobody else can access our variables
	Synchronized lock(m_mutex);
	
	x = m_xDistance;
	y = m_yDistance;
	angle = m_angle * (180 / M_PI);
}

double CoordinateSystem::d_err(double number)
{
	if ( number < 0.000001 && number > -0.000001)
	{
		if (number < 0)
			return -0.000001;
		else
			return 0.000001;
	}				
	else
		return number;
}

