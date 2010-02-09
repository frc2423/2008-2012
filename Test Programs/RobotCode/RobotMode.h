#ifndef ROBOTMODE.H
#define ROBOTMODE.H

#include <WPILib.h>

class RobotMode
{
	virtual void Main(void) = 0;
};

class mode
{
public:
	mode():
		current_mode(0)
	{};
	
	void Add(RobotMode* newMode)
	{
		modes.push_back(newMode);
	};
	
	void Next(void)
	{
		current_mode++;
	};
	
	void run(void)
	{
		if(modes.size() > 0)
		{
			int mode_mod = current_mode % modes.size();
			modes[mode_mod]->Main();
		}
	};
	
private:
	vector<RobotMode*> modes;
	current_mode;
};

