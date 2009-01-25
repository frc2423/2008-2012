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
#include <VisionAPI.h>

#include "Recorder.h"


Recorder::Recorder() :
	m_playing(false),
	m_recording(false),
	m_filename(strdup(RECORDER_DEFAULT_FILENAME))
{}

Recorder::~Recorder() 
{
	if (m_filename)
		free(m_filename);
}

void Recorder::SetFilename(const char * filename) 
{ 
	if (m_filename) free(m_filename);
	m_filename = strdup(filename); 
}

void Recorder::AddMotor(SpeedController * controller)
{
	m_motors.push_back(controller);
}


// begins playback of the recorded file if present
bool Recorder::StartPlayback()
{
	if (m_recording)
		FinishRecording();
	
	m_values.clear();
	
	printf("Attempting to begin playback from %s\n", m_filename);
	
	// load values from file into array
	file = fopen(m_filename, "r");
	
	if (!file)
	{
		fprintf(stderr, "Error opening file %s\n", m_filename);
		return false;
	}
		
	UINT32 sz = 0;
	
	if (fread(&sz, sizeof(UINT32), 1, file) != 1)
	{
		fprintf(stderr, "Error reading file count\n");
		return false;
	}
	
	if (sz <= 0)
	{
		fprintf(stderr, "No elements found in file\n");
		return false;
	}
	
	// do this stupidly, it will work	
	for (size_t i = 0; i < sz && !feof(file); i++)
	{
		m_values.push_back(RecordedBlock(m_motors.size()));
		
		for (size_t j = 0; j < m_motors.size(); ++j)
			if (fread(&m_values.back().motors[j], sizeof(float), 1, file) != 1)
			{
				if (feof(file))
					break;

				fprintf(stderr, "Error reading from file\n");
				return false;
			}
	}

	// get rid of the last one, its tricky to get right
	m_values.pop_back();
	
	m_playing = true;
	m_startTime = GetTime();
	
	// do less memory allocation later
	m_values.reserve((size_t)(RECORDER_TIME_LENGTH / RECORDER_PERIOD)); 
	
	printf("Playback started successfully\n");
	fclose(file);
	
	return true;
}
	
/*
 	Returns false when playback is complete
*/
bool Recorder::Playback()
{
	wpi_assert(m_playing);
	
	double now = GetTime();
	size_t position = (size_t)((now - m_startTime) * RECORDER_TIME_LENGTH);
	
	if (now - m_startTime > RECORDER_TIME_LENGTH || position >= m_values.size()) 
		m_playing = false;

	else if (now > m_nextActionTime)
	{	
		// playback the motor speeds
		for (size_t i = 0; i < m_motors.size(); ++i)
			m_motors[i]->Set(m_values[ position ].motors[i]);

		m_nextActionTime += PLAYBACK_PERIOD;
	}
	
	if (!m_playing)
		printf("Playback complete.\n");

	return m_playing;
}

// begins a recording
bool Recorder::StartRecording()
{
	if (m_recording)
		return false;
	
	// allow writing to file system
	Priv_SetWriteFileAllowed(1);
	
	// allow writing to file system
	Priv_SetWriteFileAllowed(1);
	
	file = fopen(m_filename, "w+");
	if (!file)
	{
		fprintf(stderr, "Recording could not start\n");
		return false;
	}
	
	printf("Recording started.\n");
	
	m_startTime = m_nextActionTime = GetTime();
	m_recording = true;
		
	return true;
}

// if record mode is enabled, this records the values needed
// to play back the movements/actions of the bot
void Recorder::Record()
{
	if (!m_recording)
		return;
	
	double now = GetTime();
	
	if (now - m_startTime > RECORDER_TIME_LENGTH) 
	{
		FinishRecording();
		return;
	}
	
	// if this is a recording period, then iterate 
	// over all motors and store the values, wait for the
	// next period. (does it make more sense to average the
	// values for the period?)
	
	if (now > m_nextActionTime)
	{
		m_values.push_back(RecordedBlock(m_motors.size()));
		
		// record the current state of all objects
		
		for (size_t i = 0; i < m_motors.size(); ++i)
		{
			// write value to array
			m_values.back().motors[i] = m_motors[i]->Get();
		}
		
		m_nextActionTime += RECORDER_PERIOD;
	}
}

// internal routine: writes the recorded data to file
void Recorder::FinishRecording()
{
	size_t sz = m_values.size();
	
	if (sz > 0)
	{
		// do this stupidly, but it will work	
		if (fwrite(&sz, sizeof(UINT32), 1, file) != 1)
			fprintf(stderr, "Error writing recording\n");
		
		const BlockIterator end = m_values.end();
		for ( BlockIterator i = m_values.begin(); i != end; i++)
			for (size_t j = 0; j < m_motors.size(); ++j)
				fwrite(&i->motors[j], sizeof(float), 1, file);
	}
	
	fclose(file);
	
	printf("Recording complete (%d sets of data written).\n", m_values.size());
	
	// clear the array, free the memory
	m_values.clear();
	
	m_recording = false;
}

