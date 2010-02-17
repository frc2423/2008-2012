#ifndef KICKER_H
#define KICKER_H

#include <WPILib.h>
#include "RobotResources.h"

class Kicker
{
public:
	Kicker(RobotResources& resources):
		m_resources(resources),
		compressor(6, 1),
		engage(1),
		release(2),
		roller(DIGITAL_SLOT, 3),
		time(),
		state(1)

	{
		compressor.Start();
		time.Start();
		roller.Set(1.0);
	}

	//Kick method controls kicker and roller. Activate_kicker parameter is needed for telling
	//kicker when user or robot needs to engage the kicker.
	void Kick(bool activate_kicker)
	{		
		
		switch(state)
		{
		case 1:
			if(activate_kicker)
			{
				roller.Set(0.0);
				engage.Set(true);
				release.Set(false);
				time.Reset();
				state = 2;
			}
			break;
		case 2:
			if(time.Get() > 0.5)
			{
				roller.Set(-1.0);
				engage.Set(false);
				release.Set(true);
				time.Reset();
				state = 3;
			}
			break;
		case 3:
			if(time.Get() > .5)
			{
				engage.Set(false);
				release.Set(false);
				state = 1;
			}
			break;
		}
	}

private:
	RobotResources& m_resources;
	Compressor compressor;
	Solenoid engage;
	Solenoid release;
	Jaguar roller;
	Timer time;
	int state;

};

#endif

