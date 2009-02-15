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

#include "WPILib.h"
#include "SwerveDrive.h"

#include "Framework/KwarqsConstants.h"
#include "Framework/math.h"

// default constructor
SwerveDrive::SwerveDrive() :
	m_servo_lf( SERVO_LF_PARAMETERS ),
	m_servo_rf( SERVO_RF_PARAMETERS ),
	m_servo_lr( SERVO_LR_PARAMETERS ),
	m_servo_rr( SERVO_RR_PARAMETERS ),
	
	m_motor_lf(MOTOR_LF_SLOT, PWM_LF_JAGUAR, ENCODER_MOTOR_LF_1, ENCODER_SERVO_LF_2 ),
	m_motor_rf(MOTOR_RF_SLOT, PWM_RF_JAGUAR, ENCODER_MOTOR_RF_1, ENCODER_SERVO_RF_2 ),
	m_motor_lr(MOTOR_LR_SLOT, PWM_LR_JAGUAR, ENCODER_MOTOR_LR_1, ENCODER_SERVO_LR_2 ),
	m_motor_rr(MOTOR_RR_SLOT, PWM_RR_JAGUAR, ENCODER_MOTOR_RR_1, ENCODER_SERVO_RR_2 )
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
	double Vy = Vty - w*Rx;
	
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
	if (fabs(speed) < 0.01 && fabs(angle) < 0.01 && fabs(rotation) < 0.01)
	{
		// this forces motors to stop and keeps the wheels pointed in the
		// same direction, much less annoying
		m_motor_lf.SetSpeed(0);
		m_motor_lr.SetSpeed(0);
		m_motor_rf.SetSpeed(0);
		m_motor_rr.SetSpeed(0);
		
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
	
	// don't divide by zero
	if (fabs(highest_speed) < 0.0001)
		speeds[0] = speeds[1] = speeds[2] = speeds[3] = 0;
	else
		for (int i = 0; i < 4; i++)
			speeds[i] /= highest_speed;
	
    
    // set the motors
	m_servo_lf.SetAngle(lf_angle);
	m_servo_lr.SetAngle(lr_angle);
	m_servo_rf.SetAngle(rf_angle);
	m_servo_rr.SetAngle(rr_angle);
	
	m_motor_lf.SetSpeed(speeds[0]);
	m_motor_lr.SetSpeed(speeds[1]);
	m_motor_rf.SetSpeed(speeds[2]);
	m_motor_rr.SetSpeed(speeds[3]);
	
}

void SwerveDrive::Stop()
{
	/// @todo: turn the wheels against the current
	/// acceleration vector and stop all motors. Is that actually
	/// a good way to do it?
	
	// if the vector is near zero, then just spread all fours 
	// and hope for the best

	m_servo_lf.SetAngle(45);
	m_servo_lr.SetAngle(315);
	m_servo_rf.SetAngle(135);
	m_servo_rr.SetAngle(225);
	
	m_motor_lf.SetSpeed(0);
	m_motor_lr.SetSpeed(0);
	m_motor_rf.SetSpeed(0);
	m_motor_rr.SetSpeed(0);
	
	
}




