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
		kicker_state(STATE_IDLE)

	{
		compressor.Start();
		time.Start();
		roller.Set(1.0);
	}

	//Kick method controls kicker and roller. Activate_kicker parameter is needed for telling
	//kicker when user or robot needs to engage the kicker.
	void Kick()
	{		
		//mutex thing here
		
		if (kicker_state == STATE_IDLE )
			kicker_state = STATE_START_KICK;
	}
		
	void KickTimerFn()
	{
		switch(kicker_state)
		{

		case STATE_DELAY_KICKER:
			roller.Set(0.0);
			time.Reset();
			state = STATE_START_KICK;
			break;	
		case STATE_START_KICK:
			if(time.Get() > rollerStop_time)
			{
				roller.Set(0.0);
				engage.Set(true);
				release.Set(false);
				time.Reset();
				state = STATE_RELEASE;
			}
			break;
		case STATE_RELEASE:
			if(time.Get() > kick_time)
			{
				roller.Set(-1.0);
				engage.Set(false);
				release.Set(true);
				time.Reset();
				state = STATE_RESET;
			}
			break;
		case STATE_RESET:
			if(time.Get() < kick_time)
				break;
			
			state = STATE_IDLE;
			
		case STATE_IDLE:
			roller.Set(-1.0);
			engage.Set(false);
			release.Set(false);
			break;
		}
	}

private:
	enum{ STATE_DELAY_KICKER, STATE_IDLE, STATE_START_KICK, STATE_RELEASE, STATE_RESET};
	
	const double kick_time = .5;
	const double rollerStop_time = .1;
	RobotResources& m_resources;
	Compressor compressor;
	Solenoid engage;
	Solenoid release;
	Jaguar roller;
	Timer time;
	int kicker_state;
	

};

#endif

