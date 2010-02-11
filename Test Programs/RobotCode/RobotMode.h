#ifndef ROBOTMODE_H
#define ROBOTMODE_H

#include <WPILib.h>

struct RobotMode
{
	virtual void Main(void) = 0;
	virtual ~RobotMode(){}
};

class Mode
{
public:
	Mode():
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
	int current_mode;
};

#endif

