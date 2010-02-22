/**
	\file 		RobotMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef ROBOTMODE_H
#define ROBOTMODE_H

#include <WPILib.h>

struct RobotMode
{
	virtual void OnEnable(void) = 0;
	virtual void Main(void) = 0;
	virtual void OnDisable(void) = 0;
	virtual ~RobotMode(){}
};

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
	
	void Next(void)
	{
		Disable();
		current_mode++;
		Enable();
	}
	
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

