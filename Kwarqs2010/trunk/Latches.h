/**
	\file 		Latches.h
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
	


#ifndef KWARQSLIB_LATCHES_H
#define KWARQsLIB_LATCHES_H

#include "DelayEvent.h"


/**
	\class LatchBase
	\brief Base class for boolean latches
	
	Do not create instances of this class, create a Latch or TimedLatch instead
*/
class LatchBase {
public:
	
	/// @return True if the latch was just set to true. False otherwise
	bool TurnedOn() const
	{
		return m_last_0 && m_last_0 != m_last_1;
	}
	
	/// @return True if the latch was just set to false. False otherwise
	bool TurnedOff() const
	{
		return !m_last_0 && m_last_0 != m_last_1;
	}
	
	/// @return True if latch is currently set, False if not set
	bool On() const
	{
		return m_last_0;
	}
	
	/// @return True if latch is not currently set, False if set
	bool Off() const
	{
		return !m_last_0;
	}
	
protected:

	/**
		\brief Default constructor for the latch
		
		@param initial_value	The initial value that the latch is set to
	*/
	LatchBase( bool initial_value = false ) :
		m_last_0(initial_value),
		m_last_1(initial_value)
	{}
	
	bool m_last_0;		// the current value set
	bool m_last_1;		// the value it was the last time
	
};


/**
 	\class Latch
 	\brief A class designed to allow you to do actions based
 	on the rising/falling edge of some particular state
 	
	Usage:

	Latch x;

	x.Set( some_button_value );

	if (x.TurnedOn())
	{
		// do first action here -- only fires once per transition
	}
	else if (x.TurnedOff())
	{
		// do action on turned off -- only fires once per transition
	}
	
	@see TimedLatch for another version
 */
class Latch : public LatchBase {
public:

	Latch(bool initial_value = false) :
		LatchBase( initial_value )
	{}

	/**
		\brief Sets the value of the latch
	*/
	void Set( bool value )
	{
		// shift the values
		m_last_1 = m_last_0;
		m_last_0 = value;
	}

private:

};


/**
	\class TimedLatch
	\brief Does the same thing as a normal latch, except that it
	only allows transitions every N seconds. This is useful to make
	sure a button doesn't get flipped too quickly and get rid of any
	noise in the button as well.
	
*/
class TimedLatch : public LatchBase {
public:

	/**
		\brief Constructor for timed latch
		
		@param initial_value	Initial value of the latch
		@param interval			Number of seconds that need to elapse
								before a transition is allowed
	*/
	TimedLatch( bool initial_value = false, double interval = 0.15) :
		LatchBase(initial_value), 
		allow_transition( interval )
	{}
	

	/**
		\brief Sets the value of the latch, but only allows
		changes every N seconds (where N was set in the 
		constructor)
	*/
	void Set(bool value)
	{
		if (m_last_0 != value && allow_transition.DoEvent())
		{
			m_last_1 = m_last_0;
			m_last_0 = value;
		}
		else
			m_last_1 = m_last_0;
	}


private:
	DelayEvent allow_transition;
};


/**
	\class StateLatch
	\brief A type of latch that is (hopefully) useful for state machine
	construction. 
	
	@warning This is not threadsafe, you should use appropriate protection
	mechanisms if using with threads
	
	@todo This is a bit clumsy to use for state machines. Need to think
	about doing this a bit better. 
*/
template <typename T>
class StateLatch  {
public:
	
	/**
		\brief Default constructor for the latch
		
		@param initial_value	The initial value that the latch is set to
	*/
	StateLatch( const T &initial_value ) :
		m_last_0(initial_value),
		m_last_1(initial_value)
	{}
	
	/// \brief Think of this as a constructor for a particular state
	/// @return True if the latch was just set to true. False otherwise
	bool EnteredState(const T &state) const
	{
		return m_last_0 == state && m_last_0 != m_last_1;
	}
	
	/// \brief Think of this as a destructor for a particular state
	/// @return True if the latch was just set to false. False otherwise
	bool LeftState(const T &state) const
	{
		return m_last_1 == state && m_last_0 != state;
	}
	
	// support implicit conversion to T
	operator T() const
	{
		return m_last_0;
	}
	
	/// assignment operator
	StateLatch<T>& operator=(const T& state)
	{
		m_last_1 = m_last_0;
		m_last_0 = state;
		
		return *this;
	}
	
	/// equality comparison operator
	bool operator==(const T& state) const
	{
		return m_last_0 == state;
	}

	/// inequality comparison operator
	bool operator!=(const T& state) const
	{
		return m_last_0 != state;
	}
	
protected:
	
	StateLatch();

	T m_last_0;		// the current value set
	T m_last_1;		// the value it was the last time
	
};


#endif
