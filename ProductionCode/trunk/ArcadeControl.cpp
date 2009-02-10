/**
	\file 		ArcadeControl.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

/*
* Portions Copyright (c) 2009 FIRST
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
#include "ArcadeControl.h"

# define M_PI           3.14159265358979323846
#include <math.h>



ArcadeControl::ArcadeControl(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	gyro(1)
{
	m_stick = Joystick::GetStickForPort(FIRST_JOYSTICK_PORT);
	gyro.SetSensitivity(0.007);
}


// implements the default arcade drive control found in WPILib
void ArcadeControl::Move()
{	
	float speed, rotateValue;
	
	if (m_stick->GetTrigger())
	{
		float y = m_stick->GetY() * -1, x = m_stick->GetX();
		
		speed = sqrt(x*x+y*y);
		
		double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 ) * -1;			
		if (desired_angle < 0) desired_angle += 360;
	
		double current_angle = fmod(gyro.GetAngle(), 360.0);
		if (current_angle < 0) current_angle += 360;
	
		rotateValue = (desired_angle - current_angle );
			
		if (rotateValue > 180)
			rotateValue = rotateValue - 360.0;
		else if (rotateValue < -180)
			rotateValue = rotateValue + 360.0;
			
		if (fabs(rotateValue) < 15.0 )
			rotateValue = 0;
		
		rotateValue = rotateValue / 90.0;
		
		if (speed < 0.01)
			rotateValue = 0.0;
		/*
		if (GetTime() - ztime > 0.1)
		{
			printf("desired: %f current: %f rotate: %f\r", 
					desired_angle, current_angle, rotateValue);
			ztime = GetTime();
		}*/
	}
	else
	{
		speed = m_stick->GetY()*-1;
		rotateValue = m_stick->GetX();
	}
	
	m_driveController->Move(speed, rotateValue);
}
