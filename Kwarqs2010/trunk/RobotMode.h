/**
	\file 		RobotMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
	
	Copyright (c) 2010 Amory Galili ("the authors")
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
	


#ifndef ROBOTMODE_H
#define ROBOTMODE_H

#include <WPILib.h>

/**
	Base class to define a particular robot mode
*/
struct RobotMode
{
	// called when this mode is enabled
	virtual void OnEnable(void) = 0;
	
	// called when this mode is being run
	virtual void Main(void) = 0;
	
	// called when this mode is disabled
	virtual void OnDisable(void) = 0;
	
	// virtual classes need virtual destructors
	virtual ~RobotMode(){}
};

/**
	A class that controls which particular mode is selected and run
*/
class ModeControl
{
public:
	ModeControl():
		current_mode(0)
	{}
	
	void Add(RobotMode* newMode)
	{
		modes.push_back(newMode);
	}
	
	// selects next mode. designed to be used with a joystick button
	void Next(void)
	{
		Disable();
		current_mode++;
		Enable();
	}
	
	// selects prev. mode. designed to be used with a joystick button. 
	void Previous(void)
	{
		Disable();
		current_mode--;
		Enable();
	}

	void Set(int mode_number, bool do_enable_disable = true)
	{
		if (do_enable_disable)
			Disable();
		
		current_mode = mode_number;
		
		if (do_enable_disable)
			Enable();
	}
	
	void Run(void)
	{
		if(modes.size() > 0)
		{
			int mode_mod = current_mode % modes.size();
			if(mode_mod < 0) mode_mod = modes.size() - mode_mod;
			modes[mode_mod]->Main();
		}
	}
	
	void Enable(void)
	{
		if(modes.size() > 0)
		{
			int mode_mod = current_mode % modes.size();
			if(mode_mod < 0) mode_mod = modes.size() - mode_mod;
			modes[mode_mod]->OnEnable();
		}
	}
	
	void Disable(void)
	{
		if(modes.size() > 0)
		{
			int mode_mod = current_mode % modes.size();
			if(mode_mod < 0) mode_mod = modes.size() - mode_mod;
			modes[mode_mod]->OnDisable();
		}
	}
	 
	 int GetMode()
	 {
		 return current_mode % modes.size();
	 }
	
private:
	vector<RobotMode*> modes;
	int current_mode;
};

#endif

