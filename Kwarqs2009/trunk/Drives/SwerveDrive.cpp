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

#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/math.h"

// default constructor
SwerveDrive::SwerveDrive(RobotChassis * chassis) :
	m_chassis(chassis),
	m_time(GetTime())
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
	/*
	if (GetTime() - m_time > 0.1)
	{		
		DriverStationLCD * lcd = DriverStationLCD::GetInstance();
		
		//lcd->Printf(DriverStationLCD::kUser_Line2, 1, "S: %.1f A: %.1f R: %.1f          ",
		//		speed, angle, rotation);
		
		lcd->Printf(DriverStationLCD::kUser_Line3, 1, "LF: %.1f %.1f %.1f          ",
			m_chassis->servo_lf.GetSetAngle(),
			m_chassis->servo_lf.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);
		
		lcd->Printf(DriverStationLCD::kUser_Line4, 1, "LR: %.1f %.1f %.1f          ",
			m_chassis->servo_lr.GetSetAngle(),
			m_chassis->servo_lr.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);
		
		lcd->Printf(DriverStationLCD::kUser_Line5, 1, "RF: %.1f %.1f %.1f          ",
			m_chassis->servo_rf.GetSetAngle(),
			m_chassis->servo_rf.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);

		lcd->Printf(DriverStationLCD::kUser_Line6, 1, "RR: %.1f %.1f %.1f          ",
			m_chassis->servo_rr.GetSetAngle(),
			m_chassis->servo_rr.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);
		
		lcd->UpdateLCD();
		m_time = GetTime();
	}
	*/
	
	
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
	double lf_speed, lr_speed, rf_speed, rr_speed;
	double lf_angle, lr_angle, rf_angle, rr_angle;
	
	// find the speed and angle of each wheel (first two params passed back)	
	CalculateWheel(lf_speed, lf_angle, Vtx, Vty, rotation, LF_DISPLACEMENT);
	CalculateWheel(lr_speed, lr_angle, Vtx, Vty, rotation, LR_DISPLACEMENT);
	CalculateWheel(rf_speed, rf_angle, Vtx, Vty, rotation, RF_DISPLACEMENT);
	CalculateWheel(rr_speed, rr_angle, Vtx, Vty, rotation, RR_DISPLACEMENT);
	
	// then limit all motors based on the highest motor speed
	double highest_speed = std::max(
		std::max( fabs(lf_speed), fabs(lr_speed) ), 
		std::max( fabs(rf_speed), fabs(rr_speed) )
	);
	
	// only need to scale if speed > 1
	if (highest_speed > 1.0 )
	{
		lf_speed /= highest_speed;
		lr_speed /= highest_speed;
		rf_speed /= highest_speed;
		rr_speed /= highest_speed;
	}
	
	// calculate the shortest path to the setpoint
	ShortestPath(lf_speed, lf_angle, m_chassis->servo_lf.GetCurrentAngle());
	ShortestPath(lr_speed, lr_angle, m_chassis->servo_lr.GetCurrentAngle());
	ShortestPath(rf_speed, rf_angle, m_chassis->servo_rf.GetCurrentAngle());
	ShortestPath(rr_speed, rr_angle, m_chassis->servo_rr.GetCurrentAngle());
	
    
    // set the motors
	m_chassis->servo_lf.SetAngle(lf_angle);
	m_chassis->servo_lr.SetAngle(lr_angle);
	m_chassis->servo_rf.SetAngle(rf_angle);
	m_chassis->servo_rr.SetAngle(rr_angle);
	
	m_chassis->motor_lf.SetSpeed((float)lf_speed);
	m_chassis->motor_lr.SetSpeed((float)lr_speed);
	m_chassis->motor_rf.SetSpeed((float)rf_speed);
	m_chassis->motor_rr.SetSpeed((float)rf_angle);
}

void SwerveDrive::Stop()
{
	/// @todo: turn the wheels against the current
	/// acceleration vector and stop all motors. Is that actually
	/// a good way to do it?
	
	// if the vector is near zero, then just spread all fours 
	// and hope for the best

	m_chassis->servo_lf.SetAngle(45);
	m_chassis->servo_lr.SetAngle(315);
	m_chassis->servo_rf.SetAngle(135);
	m_chassis->servo_rr.SetAngle(225);
	
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




