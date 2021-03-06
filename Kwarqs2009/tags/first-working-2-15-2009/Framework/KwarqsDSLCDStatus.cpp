/**
	\file 		KwarqsDSLCDStatus.h
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

#include "Notifier.h"
#include <WPILib.h>

#include "KwarqsDSLCDStatus.h"
#include "PositionInformation.h"

KwarqsDSLCDStatus::KwarqsDSLCDStatus()
{
	m_notifier = new Notifier(KwarqsDSLCDStatus::OnInterval, NULL);
	m_notifier->StartPeriodic(0.25);
}

KwarqsDSLCDStatus::~KwarqsDSLCDStatus()
{
	if (m_notifier) delete m_notifier;
}
	

// this function does the update
void KwarqsDSLCDStatus::OnInterval(void * param)
{
	DriverStationLCD * lcd = DriverStationLCD::GetInstance();
	PositionInformation * pos = PositionInformation::GetInstance();
	
	// do any updates necessary
	lcd->Printf(DriverStationLCD::kMain_Line6, 1, 
			"%.1f@ Uncal: %d",
			pos->GetNormalizedAngle(),
			KwarqsWheelServo::UncalibratedServoCount()
	);
	
	// then write it
	lcd->UpdateLCD();
}
