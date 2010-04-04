/**
	\file 		DelayEvent.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
	
	Copyright (c) 2010 Dustin Spicuzza ("the authors")
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the authors nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef KWARQSLIB_DELAY_EVENT_H
#define KWARQSLIB_DELAY_EVENT_H



/**
	\class DelayEvent
	\brief Simple class to abstract delayed actions away
	
	This class is particularly useful for simulating wait conditions
	in a single thread. Every time you call DoEvent(), it checks to
	see if the time to wait has expired, and if it has then it
	
	Use cases: debouncing a joystick button input, or a limit switch
	on the drivers station. 
	
	Usage Example:
	
	double N = 0.005;			// some interval
	DelayEvent event( N );
	
	while (true)
	{
		if ( event.DoEvent() )
		{
			// this gets executed every N seconds
		}
	}
	
*/
class DelayEvent {
public:

	DelayEvent(double interval = 0.15) :
		m_interval(interval),
		m_lastUpdate(GetTime())
	{}

	/// returns true if the interval has expired
	bool DoEvent()
	{
		double current_time = GetTime();
		if (current_time - m_lastUpdate > m_interval)
		{
			m_lastUpdate = current_time;
			return true;
		}
		
		return false;
	}
	
	void Reset()
	{
		m_lastUpdate = GetTime();
	}

private:
	double m_interval;
	double m_lastUpdate;
};

#endif
