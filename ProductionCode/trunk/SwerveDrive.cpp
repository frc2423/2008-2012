/**
	\file 		SwerveDrive.cpp
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-02-10 00:30:27 -0500 (Tue, 10 Feb 2009) $
	\version 	$Rev: 118 $
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

// default constructor
SwerveDrive::SwerveDrive() :
	m_servo_lf(SERVO_LF_SLOT, PWM_LF_VICTOR, ENCODER_SERVO_LF_1, ENCODER_SERVO_LF_2, SERVO_F_SCALE, SERVO_F_TICKS ),
	m_servo_rf(SERVO_RF_SLOT, PWM_RF_VICTOR, ENCODER_SERVO_RF_1, ENCODER_SERVO_RF_2, SERVO_R_SCALE, SERVO_R_TICKS ),
	m_servo_lr(SERVO_LR_SLOT, PWM_LR_VICTOR, ENCODER_SERVO_LR_1, ENCODER_SERVO_LR_2, SERVO_F_SCALE, SERVO_F_TICKS ),
	m_servo_rr(SERVO_RR_SLOT, PWM_RR_VICTOR, ENCODER_SERVO_RR_1, ENCODER_SERVO_RR_2, SERVO_R_SCALE, SERVO_R_TICKS ),
	
	m_motor_lf(MOTOR_LF_SLOT, PWM_LF_JAGUAR, ENCODER_MOTOR_LF_1, ENCODER_SERVO_LF_2 ),
	m_motor_rf(MOTOR_RF_SLOT, PWM_RF_JAGUAR, ENCODER_MOTOR_RF_1, ENCODER_SERVO_RF_2 ),
	m_motor_lr(MOTOR_LR_SLOT, PWM_LR_JAGUAR, ENCODER_MOTOR_LR_1, ENCODER_SERVO_LR_2 ),
	m_motor_rr(MOTOR_RR_SLOT, PWM_RR_JAGUAR, ENCODER_MOTOR_RR_1, ENCODER_SERVO_RR_2 )
{}

void SwerveDrive::Move(double &speed, double &angle, double &rotateValue)
{
	// note: currently a rather braindead implementation, mostly because it
	// assumes wheel reaction time is instantaneous
   
    if (fabs(rotation) < 0.01)
    {
        // no rotation
		
		m_servo_lf.SetAngle(angle);
		m_servo_lf.SetAngle(angle);
		m_servo_lf.SetAngle(angle);
		m_servo_lf.SetAngle(angle);
		
		m_motor_lf.SetSpeed(speed);
		m_motor_lr.SetSpeed(speed);
		m_motor_rf.SetSpeed(speed);
		m_motor_rr.SetSpeed(speed);
		
	}
	else
	{
		// we need some rotation code here... punt for now
		
		m_motor_lf.SetSpeed(0);
		m_motor_lr.SetSpeed(0);
		m_motor_rf.SetSpeed(0);
		m_motor_rr.SetSpeed(0);
    }


	// todo: ideas taken from Ian Mackenzie's ppt: 
	// http://www.firstroboticscanada.org/site/files/workshops/omnidirectional.pdf
	
	/*
		Thoughts and notes
		
		We should calculate an ideal speed and angle for each wheel (ie, 
		our goal), and tell the wheel servos to go there. However, we must
		set the velocities of the wheels based on the current angle of the 
		wheels so that we don't go in weird directions
		
		Question: is this actually true? Should we just sit until the wheels
		turn (this seems like a bad idea)
		
	*/
}

