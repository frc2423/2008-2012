/**
	\file 		PositionInformation.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$

	Copyright (c) 2010 Amory Galili, Dustin Spicuzza ("the authors")
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the authors nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PositionInformation.h"
#include <cmath>

#include <limits>
#undef min
#undef max


PositionInformation::PositionInformation(RobotResources& resources, const double wheel_base):
	m_theta(0),
	m_wheelBase(wheel_base),
	m_notifier(PositionInformation::TimerFn, this),
	m_resources(resources)
{
	m_leftMeters = m_resources.webdma.CreateDoubleProxy("Coordinate System", "left meters",
		DoubleProxyFlags()
			.default_value(0)
			.minval(std::numeric_limits<double>::min())
			.maxval(std::numeric_limits<double>::max())
			.step(1)
			.readonly()
	);
	
	m_rightMeters = m_resources.webdma.CreateDoubleProxy("Coordinate System", "right meters",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	m_x = m_resources.webdma.CreateDoubleProxy("Coordinate System", "X",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	m_y = m_resources.webdma.CreateDoubleProxy("Coordinate System", "Y",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	m_display_angle = m_resources.webdma.CreateDoubleProxy("Coordinate System", "Angle",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
	
	m_l = m_resources.webdma.CreateIntProxy("Coordinate System", "RawL", 
			IntProxyFlags().readonly()
	);
	
	m_r = m_resources.webdma.CreateIntProxy("Coordinate System", "RawR",
			IntProxyFlags().readonly()
	);
	
	m_gyro_angle = m_resources.webdma.CreateDoubleProxy("Coordinate System", "Gyro Angle", 
			DoubleProxyFlags().readonly()
	);
	
	// create a mutex to allow us to synchronize access to variables
	// since we're going to be running in multiple threads
	m_mutex = semMCreate(0);
}

/*
 * 	wheel_radius - Radius of the wheel we are using (in meters)
 *  wheel_base	 - Distance between the wheels (in meters)
 */

void PositionInformation::Start()
{
	m_notifier.StartPeriodic(.0025);
}

// callback function for the Notifier 
void PositionInformation::TimerFn(void * param)
{ 
	((PositionInformation*)param)->CalculationTimerFn(); 
}

// this is called every 50ms and performs calculations

/*
	ComputeNewPosition
	
	Assumes a uniform motion for each wheel over some period
	with no slippage. 

	@param l		Left meters
	@param r		Right meters

	Left meters/right meters:
		If they are positive, the wheel is moving in the forward
		direction
*/
void PositionInformation::ComputeNewPosition(double l, double r)
{
	double dx, dy, dtheta;

	dtheta = ( r - l ) / m_wheelBase;
	
	// dy = ((r+l)/(2.0 *dtheta)) * sin(dtheta);
	// dx = - ((r+l)/(2.0 * dtheta)) * (1.0 - cos(dtheta))
	//
	// BUT
	//
	// you need to expand these out using a taylor series approximation
	// to avoid divide by zero errors. So we end up with a more
	// complicated expression that looks like this:
	//
	// dy = ((r+l)/2.0 ) * ( 1 - dtheta^2 / 3! + dtheta^4/5! );
	// dx = - ((r+l)/2.0) * (dtheta/2! - dtheta^3/4! + dtheta^5/6!)
	//
	
	if (fabs(dtheta) > M_PI/36.0)
	{
		// anything more than five degrees: simple expression
		dy = ((r+l)/(2.0 * dtheta)) * sin(dtheta);
		dx = - ((r+l)/(2.0 * dtheta)) * (1.0 - cos(dtheta));
	}
	else
	{
		// anything less than five degrees: more complex expression
		dy = ((r+l)/2.0 ) * ( 1.0 - (dtheta*dtheta)/ 6.0 + (dtheta*dtheta*dtheta*dtheta)/120.0);
		dx = - ((r+l)/2.0) * (dtheta/2.0 - (dtheta*dtheta*dtheta)/24.0 + (dtheta*dtheta*dtheta*dtheta*dtheta)/720.0);
	}

	TransformCoordinates( dx, dy, dtheta );
	
	m_display_angle = m_theta * (180.0 / M_PI);
	
	m_gyro_angle = m_resources.gyro.GetAngle();
}

/**
	Internal function
	
	Transforms coordinates to the original frame of reference
*/
void PositionInformation::TransformCoordinates(double dx, double dy, double dtheta)
{
	m_x = m_x + dx * cos(m_theta) - dy * sin(m_theta);
	m_y = m_y + dx * sin(m_theta) + dy * cos(m_theta);
	
	// calculate this last
	m_theta = m_theta + dtheta;
}



void PositionInformation::CalculationTimerFn ()
{
	// make sure that nobody else can access our variables
	Synchronized lock(m_mutex);
	
	m_l = m_resources.leftEncoder.GetRaw();
	m_r = m_resources.rightEncoder.GetRaw();
	
	// Store encoder values so we can use them next time.
	double prev_leftMeters = m_leftMeters;
	double prev_rightMeters = m_rightMeters;
	
	m_leftMeters = m_resources.leftEncoder.GetDistance();
	m_rightMeters = - m_resources.rightEncoder.GetDistance();
	
	double leftDistance = m_leftMeters - prev_leftMeters;
	double rightDistance = m_rightMeters - prev_rightMeters;
	
	ComputeNewPosition(leftDistance, rightDistance);
}

// returns x and y relative to robot starting posititon, and angle in degrees, by reference
void PositionInformation::getData(double &x, double &y, double &angle)
{
	// make sure that nobody else can access our variables
	Synchronized lock(m_mutex);
	
	x = m_x;
	y = m_y;
	angle = fmod(m_theta * (180.0 / M_PI), 360.0);
}



