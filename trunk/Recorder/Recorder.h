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

#ifndef _RECORDER_H
#define _RECORDER_H

#define RECORDER_TIME_LENGTH 15
#define RECORDER_PERIOD 0.05
#define RECORDER_DEFAULT_FILENAME "playback.log"


// holds recorded values, add more types
// as necessary
struct RecordedBlock {
	float * motors;
	
	RecordedBlock(size_t msz)
	{
		motors = new float[msz];
	}
	
	~RecordedBlock() {
		delete [] motors;
	}
};


/**
	\class Recorder
	\brief A class that can record movements in a file
	
	There are a number of unresolved questions, but at least this
	will demonstrate the basic concept on how something like this
	should work. The primary question is whether one should be 
	recording the results or the inputs. With a more complex 
	system, it may make more sense to just capture the input values.
	It would be a bit more involved however.

	@todo Usage notes
	@todo More error checking
*/
class Recorder {
public:
	
	Recorder();
	~Recorder();
	
	void SetFilename(const char * filename) 
	{ 
		if (m_filename) free(m_filename);
		m_filename = strdup(filename); 
	}
	
	void AddMotor(SpeedController * controller)
	{
		m_motors.push_back(controller);
	}
	
	// begins playback
	bool StartPlayback();
	
	// writes values to the stored file
	bool Playback();
	
	// begins recording
	bool StartRecording();
	
	// writes stored device values to file
	void Record();
	
private:
	
	void FinishRecording();
	
	typedef vector<SpeedController *> 	MotorContainer;
	typedef MotorContainer::iterator	MotorIterator;
	
	typedef vector<RecordedBlock> 		RecordedBlocks;
	typedef RecordedBlocks::iterator 	BlockIterator;
	
	RecordedBlocks m_values;
	
	DriverStation * m_ds;
	MotorContainer m_motors;
	
	bool m_playing;
	bool m_recording;
	
	double m_startTime;
	double m_nextActionTime;
	
	size_t m_playbackPosition;
	
	char * m_filename;
	
	FILE * file;
};

#endif
