/**
	\file 		KwarqsTelemetry.h
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

#ifndef KWARQSLIB_KWARQSTELEMETRY_H
#define KWARQSLIB_KWARQSTELEMETRY_H

#include "DataFile.h"
#include <vector>

/**
	\class KwarqsTelemetry
	\brief A class to do asynchronous logging of telemetry data to file
	
	This just tries to get data to the file as much as it can. One
	of the obvious caveats is that the end of the file may end up
	being corrupted. 
	
	This can record to disk any type that can be serialized using the serialization
	pattern found in serialization.h
	
	@tparam Data			type of data that needs to be stored
	@tparam flush_period	writes to disk every period milliseconds
*/
template <typename Data, size_t flush_period = 500>
class KwarqsTelemetry {
public:

	/// standard constructor
	KwarqsTelemetry() :
		m_notifier(NULL),
		m_mutex( semBCreate(SEM_Q_PRIORITY, SEM_FULL) )
	{
		// create a unique filename that contains the date in it
		char filename[128];
		time_t the_time = time(NULL);
		strftime(filename, 128, "Telemetry-%Y%m%d-%H%M-%S.dat", localtime(&the_time));
			
		// try to open a new file
		//if (m_file.Open(filename, DataFile::TruncateAndWrite))
		//{
		//	m_notifier = new Notifier(KwarqsTelemetry::StaticTimerFn, this);
		//	m_notifier->StartPeriodic((double)flush_period / 1000.0);
		//}
	}

	~KwarqsTelemetry()
	{
		if (m_notifier) delete m_notifier;
	}

	/// queue up data to be recorded
	void AddData(const Data &data)
	{
		//Synchronized sync(m_mutex);
		//m_data.push_back(data);
	}

private:

	/// called every flush_period seconds to write stuff to disk
	static void StaticTimerFn(void * param)
	{
		((KwarqsTelemetry*)param)->TimerFn();
	}
	
	/// does the writing to disk
	void TimerFn()
	{
		std::vector<Data> data;
		data.reserve(32);
		
		{
			// grab the array of data, use swap because its
			// fast and we don't want to hold up the rest of the
			// app while waiting for the write to complete
			Synchronized sync(m_mutex);
			data.swap(m_data);
		}
		
		// write it all to disk
		for (size_t i = 0; i < data.size(); i++)
			m_file.Write(data[i]);
		
		// flush it to ensure it gets written (should we sync?)
		m_file.Flush();
	}

	Notifier * m_notifier;
	DataFile m_file;
	
	std::vector<Data> m_data;
	
	SEM_ID m_mutex;
	
};

#endif
