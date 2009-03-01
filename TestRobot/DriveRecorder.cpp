/**
	\file 		DriveRecorder.cpp
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-01 01:00:41 -0500 (Sun, 01 Mar 2009) $
	\version 	$Rev: 244 $
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
#include "DriveRecorder.h"

#include "KwarqsLib/KwarqsConstants.h"


DriveRecorder::DriveRecorder() :
	m_stick(DRIVE_RECORDER_STICK),
	m_ds(DriverStation::GetInstance()),
	lcd(DriverStationLCD::GetInstance()),
	m_notifier(new Notifier(DriveRecorder::StaticTimerFn, this)),
	
	m_recording(false),
	m_enabled(false),
	m_lastupdate(0)
{
	m_mutex = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
}

DriveRecorder::~DriveRecorder()
{
	delete m_notifier;
}


void DriveRecorder::Move(double &speed, double &angle, double &rotation, bool &stop)
{
	Synchronized sync(m_mutex);

	if (!m_recording)
	{
		// if the right switches are set, start it
		if (m_stick.GetTop() && m_ds->GetDigitalIn(DRIVE_RECORDER_SWITCH))
		{
			// create a unique filename that contains the date in it
			char filename[128];
			time_t the_time = time(NULL);
			strftime(filename, 128, "DriveRecorder-%Y%m%d-%H%M-%S.dat", localtime(&the_time));
		
			// try to open a new file
			if (m_file.Open(filename, DataFile::TruncateAndWrite))
			{
				m_recording = true;
				m_enabled = false;
				m_record_mode_endtime = GetTime() + 18;
				m_notifier->StartPeriodic(DRIVERECORDER_PERIOD);
			}
		}
	}
	
	if (m_recording)
	{
		double time_left = m_record_mode_endtime - GetTime();

		// done
		if (time_left < 0 || !m_ds->GetDigitalIn(DRIVE_RECORDER_SWITCH))
		{
			FlushDataToFile();
		}
		else if (time_left < 15)
		{
			m_enabled = true;
		
			// display countdown on LCD
			if (m_lcdDelay.DoEvent())
				lcd->PrintfLine(DriverStationLCD::kMain_Line, "Recording left: %d", (int)ceil(time_left));
		}
		else
		{
			// keep it stopped when counting down
			speed = 0;
			angle = 0;
			rotation = 0;
			stop = false;
		
			// display countdown on LCD
			if (m_lcdDelay.DoEvent())
				lcd->PrintfLine(DriverStationLCD::kMain_Line, "Starting in %d", (int)ceil(time_left - 15));
		}
		
		// store stuff for the notifier to pick up
		m_last_data.speed = speed;
		m_last_data.angle = angle;
		m_last_data.rotation = rotation;
		m_last_data.stop = stop;
	}

}

void DriveRecorder::FlushDataToFile()
{			
	m_notifier->Stop();
	m_recording = false;
	m_enabled = false;
	
	size_t sz = m_data.size();
	
	if (sz > 0)
	{
		// do this stupidly, but it will work
		if (!m_file.Write(sz))
			fprintf(stderr, "Error writing recording\n");
		else
		{
			// assume there are never errors.. :o
			for (size_t i = 0; i < sz; i++)
				m_data[i].DoWriteWith(m_file);
		}

		m_file.Close();
		printf("Recording complete (%d sets of data written).\n", m_data.size());
	}
	
	// clear the array, free the memory
	m_data.clear();
}


void DriveRecorder::TimerFn()
{
	Synchronized sync(m_mutex);

	if (m_enabled)
		m_data.push_back( m_last_data );
}


