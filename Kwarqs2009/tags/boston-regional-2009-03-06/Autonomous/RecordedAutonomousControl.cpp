/**
	\file 		RecordedAutonomousControl.cpp
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

 
#include <WPILib.h>
#include "RecordedAutonomousControl.h"


RecordedAutonomousControl::RecordedAutonomousControl(KwarqsDriveController * controller) :
	KwarqsMovementControl(controller),
	m_notifier(RecordedAutonomousControl::StaticTimerFn, this),
	m_mutex(semBCreate(SEM_Q_PRIORITY, SEM_FULL))
{}

void RecordedAutonomousControl::OnEnable()
{
	m_data.clear();

	// set this to default to zero movement
	m_current_data.Reset();
	m_idx = 0;
	
	DataFile file;
	
	if (!file.Open("AutonomousMode.dat", DataFile::ReadOnly))
	{
		fprintf(stderr, "Error opening AutonomousMode.dat file\n");
		return;
	}
	
	size_t sz = 0;
	
	// read count first
	if (!file.Read(sz))
	{
		fprintf(stderr, "Error reading file count\n");
		return;
	}
	
	// ensure it isn't zero
	if (sz == 0)
	{
		fprintf(stderr, "No elements found in file\n");
		return;
	}
	
	// do this stupidly, it will work	
	for (size_t i = 0; i < sz && !file.Eof(); i++)
	{
		if (!m_current_data.DoReadWith(file))
		{
			if (!file.Eof())
				fprintf(stderr, "Error reading from file\n");
				
			break;
		}
		
		// copy the data onto the output array
		m_data.push_back(m_current_data);
	}
		
	// setup the first data point
	if (m_data.size())
		m_current_data = m_data[m_idx++];

	// and.. begin the madness
	m_notifier.StartPeriodic(DRIVERECORDER_PERIOD);
}

void RecordedAutonomousControl::OnDisable()
{
	m_notifier.Stop();
}

// executed every DRIVERECORDER_PERIOD seconds
void RecordedAutonomousControl::TimerFn()
{
	Synchronized sync(m_mutex);

	if (m_idx < m_data.size())
	{
		// read in the data
		m_current_data = m_data[m_idx++];
	}
	else
		m_current_data.Reset();
}

/// just reads the last data value out
void RecordedAutonomousControl::Move()
{
	Synchronized sync(m_mutex);

	m_driveController->Move(
		m_current_data.speed, 
		m_current_data.angle, 
		m_current_data.rotation, 
		m_current_data.stop
	);
}

