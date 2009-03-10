/**
	\file 		SwerveDrive.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

/*
 * Copyright (c) 2009 Dustin Spicuzza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Dustin Spicuzza nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DUSTIN SPICUZZA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DUSTIN SPICUZZA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// for definition of std::max
#include <stdio.h>

#include <WPILib.h>

#include "SwerveDrive.h"

#include "../KwarqsLib/PositionInformation.h"
#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/math.h"

// default constructor
SwerveDrive::SwerveDrive(RobotChassis * chassis) :
	m_chassis(chassis),
	m_time(GetTime()),
	m_stick(FIRST_JOYSTICK_PORT)
{}

/*
	Calculates the parameters for an individual wheel
	
	Set as inline function for speed
*/
static inline
void CalculateWheel(
	double &magnitude, double &angle,
	double Vtx, double Vty, double w,
	double Rx, double Ry)
{
	double Vx = Vtx - w*Ry;
	double Vy = Vty + w*Rx;
	
	// return as polar coordinates
	magnitude = __hypot(Vx, Vy); 
	angle = (atan2(Vy, Vx)*180)/M_PI - 90.0;
}





void SwerveDrive::Move(
	double &speed, 
	double &angle, 
	double &rotation,
	bool &stop
)
{
	if (stop)
	{
		Stop();
		return;
	}
	
	/*
		Thoughts and notes
		
		We should calculate an ideal speed and angle for each wheel (ie, 
		our goal), and tell the wheel servos to go there. However, we must
		set the velocities of the wheels based on the current angle of the 
		wheels so that we don't go in weird directions
		
		Question: is this actually true? Should we just sit until the wheels
		turn (this seems like a bad idea)
		
		Currently, neither of those is implemented.
		
	*/
	
	// special case: doing nothing
	if (fabs(speed) < 0.0001 && fabs(angle) < 0.0001 && fabs(rotation) < 0.0001)
	{
		// this forces motors to stop and keeps the wheels pointed in the
		// same direction, much less annoying
		m_chassis->motor_lf.SetSpeed(0);
		m_chassis->motor_lr.SetSpeed(0);
		m_chassis->motor_rf.SetSpeed(0);
		m_chassis->motor_rr.SetSpeed(0);
		
		return;
	}
	
	
	// set limitations on speed
	if (speed < -1.0)
		speed = -1.0;
	else if (speed > 1.0)
		speed = 1.0;
	
	// set limitations on rotation
	if (rotation < -1.0)
		rotation = -1.0;
	else if (rotation > 1.0)
		rotation = 1.0;
 
	// convert speed/angle to Vx/Vy (offset angle by 90 degrees)
	double Vtx = speed * cos( ((angle+90.0) * M_PI)/180.0 );
	double Vty = speed * sin( ((angle+90.0) * M_PI)/180.0 );
	
	// LF, LR, RF, RR
	double speeds[] = { 0.0, 0.0, 0.0, 0.0 };
	double lf_angle, lr_angle, rf_angle, rr_angle;
	
	// find the speed and angle of each wheel (first two params passed back)	
	CalculateWheel(speeds[0], lf_angle, Vtx, Vty, rotation, LF_DISPLACEMENT);
	CalculateWheel(speeds[1], lr_angle, Vtx, Vty, rotation, LR_DISPLACEMENT);
	CalculateWheel(speeds[2], rf_angle, Vtx, Vty, rotation, RF_DISPLACEMENT);
	CalculateWheel(speeds[3], rr_angle, Vtx, Vty, rotation, RR_DISPLACEMENT);
	
	// then limit all motors based on the highest motor speed
	double highest_speed = 0;
	
	for (int i = 0; i < 4; i++)
		if (fabs(speeds[i]) > highest_speed)
			highest_speed = fabs(speeds[i]);
	
	// only need to scale if speed > 1
	if (highest_speed > 1.0 )
	{
		// scale each speed by the highest speed
		for (int i = 0; i < 4; i++)
			speeds[i] /= highest_speed;
	}
	
	// calculate the shortest path to the setpoint
	ShortestPath(speeds[0], lf_angle, m_chassis->servo_lf.GetCurrentAngle());
	ShortestPath(speeds[1], lr_angle, m_chassis->servo_lr.GetCurrentAngle());
	ShortestPath(speeds[2], rf_angle, m_chassis->servo_rf.GetCurrentAngle());
	ShortestPath(speeds[3], rr_angle, m_chassis->servo_rr.GetCurrentAngle());
	
    
    // set the motors
	m_chassis->servo_lf.SetAngle(lf_angle);
	m_chassis->servo_lr.SetAngle(lr_angle);
	m_chassis->servo_rf.SetAngle(rf_angle);
	m_chassis->servo_rr.SetAngle(rr_angle);
	
	m_chassis->motor_lf.SetSpeed(speeds[0]);
	m_chassis->motor_lr.SetSpeed(speeds[1]);
	m_chassis->motor_rf.SetSpeed(speeds[2]);
	m_chassis->motor_rr.SetSpeed(speeds[3]);
}

void SwerveDrive::Stop()
{
	double y = m_stick.GetY() * -1, x = m_stick.GetX();
		
	double speed = __hypot(x, y);
	double angle = (atan2(y, x) * (180/M_PI) - 90.0 );			
	
	double lf_angle, lr_angle, rf_angle, rr_angle;
	
	if (fabs(speed) < 0.00001)
	{
		// pick a default if the driver isn't pointing
		lf_angle = 45;
		lr_angle = 315;
		rf_angle = 135;
		rr_angle = 225;
	}
	else
	{
		// translate the direction the driver is pointing
		angle = PositionInformation::GetInstance()->TranslateFieldToRobotAngle(angle) - 90;
	
		lf_angle = lr_angle = rf_angle = rr_angle = angle;
	}
	
	// find the shortest path to that spot, and do it
	ShortestPath(speed, lf_angle, m_chassis->servo_lf.GetCurrentAngle());
	ShortestPath(speed, lr_angle, m_chassis->servo_lr.GetCurrentAngle());
	ShortestPath(speed, rf_angle, m_chassis->servo_rf.GetCurrentAngle());
	ShortestPath(speed, rr_angle, m_chassis->servo_rr.GetCurrentAngle());

	
	m_chassis->servo_lf.SetAngle(lf_angle);
	m_chassis->servo_lr.SetAngle(lr_angle);
	m_chassis->servo_rf.SetAngle(rf_angle);
	m_chassis->servo_rr.SetAngle(rr_angle);
		
	m_chassis->motor_lf.SetSpeed(0);
	m_chassis->motor_lr.SetSpeed(0);
	m_chassis->motor_rf.SetSpeed(0);
	m_chassis->motor_rr.SetSpeed(0);
}

// returns the shortest path to an angle, adjusts speed also
void SwerveDrive::ShortestPath(double &speed, double &angle, double current_angle )
{
	double alternate_angle = angle + 180 > 360 ? angle - 180 : angle + 180;

	double error1 = angle - current_angle;
	double error2 = alternate_angle - current_angle;

	if (fabs(error1) > 180)
	{
		if (error1 < 0)
			error1 += 360;
		else
			error1 -= 360;
	}
	
	error1 = fabs(error1);
	error2 = fabs(error2);

	// if the other way is quicker, then use that instead
	if (error1 > error2)
	{
		angle = alternate_angle;
		speed = speed * -1;
	}
}





