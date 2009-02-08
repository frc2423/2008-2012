/**
	\file 		ArcadeDrive.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

/*
* Copyright (c) 2009 FIRST
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the FIRST nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY FIRST AND CONTRIBUTORS``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY NONINFRINGEMENT AND FITNESS FOR A PARTICULAR 
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FIRST OR CONTRIBUTORS BE LIABLE FOR 
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "WPILib.h"
#include "ArcadeDrive.h"

#include "Framework/KwarqsDevices.h"

// default constructor
ArcadeDrive::ArcadeDrive()
{
	m_leftMotor = new Jaguar(LEFT_MOTOR_PWM);
	m_rightMotor = new Jaguar(RIGHT_MOTOR_PWM);
}

// destructor
ArcadeDrive::~ArcadeDrive()
{
	delete m_leftMotor;
	delete m_rightMotor;
}

void ArcadeDrive::Move(float &speed, float &rotateValue)
{
	// local variables to hold the computed PWM values for the motors
	float leftMotorSpeed;
	float rightMotorSpeed;

	speed = Limit(speed) * -1;
	rotateValue = Limit(rotateValue);

	if (speed > 0.0)
	{
		if (rotateValue > 0.0)
		{
			leftMotorSpeed = speed - rotateValue;
			rightMotorSpeed = max(speed, rotateValue);
		}
		else
		{
			leftMotorSpeed = max(speed, -rotateValue);
			rightMotorSpeed = speed + rotateValue;
		}
	}
	else
	{
		if (rotateValue > 0.0)
		{
			leftMotorSpeed = - max(-speed, rotateValue);
			rightMotorSpeed = speed + rotateValue;
		}
		else
		{
			leftMotorSpeed = speed - rotateValue;
			rightMotorSpeed = - max(-speed, -rotateValue);
		}
	}
	
	// set the motor speeds after limiting them
	leftMotorSpeed = Limit(leftMotorSpeed);
	rightMotorSpeed = -Limit(rightMotorSpeed);
	
	m_leftMotor->Set(leftMotorSpeed);
	m_rightMotor->Set(rightMotorSpeed);
}


/**
 * Limit motor values to the -1.0 to +1.0 range.
 */
float ArcadeDrive::Limit(float num)
{
	if (num > 1.0)
	{
		return 1.0;
	}
	if (num < -1.0)
	{
		return -1.0;
	}
	return num;
}

