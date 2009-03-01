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

#include <WPILib.h>
#include "ArcadeControl.h"


ArcadeControl::ArcadeControl(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	m_stick(FIRST_JOYSTICK_PORT)
{
}

// implements the default arcade drive control found in WPILib
void ArcadeControl::Move()
{
	double moveValue = m_stick.GetY()*-1, rotateValue = m_stick.GetX();

	if (m_stick.GetTrigger())
	{
		// square the inputs (while preserving the sign) to increase fine control while permitting full power
		if (moveValue >= 0.0)
		{
			moveValue = (moveValue * moveValue);
		}
		else
		{
			moveValue = -(moveValue * moveValue);
		}
		if (rotateValue >= 0.0)
		{
			rotateValue = (rotateValue * rotateValue);
		}
		else
		{
			rotateValue = -(rotateValue * rotateValue);
		}
	}
	
	m_driveController->Move(moveValue, 0, rotateValue, false);
}
