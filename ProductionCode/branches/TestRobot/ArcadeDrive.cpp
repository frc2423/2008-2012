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

#include "Framework/KwarqsConstants.h"

// default constructor
ArcadeDrive::ArcadeDrive() :
	m_leftMotor(MOTOR_L_PARAMETERS, DriverStationLCD::kUser_Line2),
	m_rightMotor(MOTOR_R_PARAMETERS, DriverStationLCD::kUser_Line3),
	m_encoder(9,10),
	m_tm(GetTime())
{
	//m_encoder.SetDistancePerPulse(.001521);
	m_encoder.SetDistancePerPulse(.001239);
	m_encoder.SetReverseDirection(true);
	m_encoder.Start();
}

// destructor
ArcadeDrive::~ArcadeDrive()
{}

void ArcadeDrive::Move(double &speed, double &heading, double &rotateValue, bool &stop)
{
	// local variables to hold the computed PWM values for the motors
	double leftMotorSpeed;
	double rightMotorSpeed;

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
	
	m_leftMotor.SetSpeed(leftMotorSpeed, m_encoder.GetRate());
	m_rightMotor.SetSpeed(rightMotorSpeed, m_encoder.GetRate());
	
	if (GetTime() - m_tm)
	{
		DriverStationLCD::GetInstance()->PrintfLine(
				DriverStationLCD::kUser_Line5, "%.2f %d",
				m_encoder.GetDistance(),
				m_encoder.GetRaw());
		m_tm = GetTime();
	}
	
}


/**
 * Limit motor values to the -1.0 to +1.0 range.
 */
double ArcadeDrive::Limit(double num)
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

