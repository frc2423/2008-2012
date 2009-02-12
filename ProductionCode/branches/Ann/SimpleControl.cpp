/**
	\file 		SimpleControl.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
	
	Simple control of a 'swerve drive' style robot. Requires a joystick that
	has a twist in it. 
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


#include <WPILib.h>

#include "SimpleControl.h"

#include "Framework/math_definitions.h"
#include "Framework/KwarqsConstants.h"


SimpleControl::SimpleControl(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController)
{
	m_stick = Joystick::GetStickForPort(FIRST_JOYSTICK_PORT);
}


// implements the default arcade drive control found in WPILib
void SimpleControl::Move()
{	
	double y = m_stick->GetY() * -1, x = m_stick->GetX();
		
	double speed = sqrt(x*x+y*y);
	
	double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 ) * -1;			
	if (desired_angle < 0) desired_angle += 360;
	
	if (fabs(speed) < 0.01)
		desired_angle = 0.0;
	
	m_driveController->Move(speed, desired_angle, m_stick->GetTwist());
}