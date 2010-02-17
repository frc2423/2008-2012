#ifndef KICKER_H
#define KICKER_H

#include <WPILib.h>
#include "RobotResources.h"


class Kicker
{
public:
	Kicker(RobotResources& resources):
		m_resources(resources),
		m_notifier(Kicker::TimerFn, this),
		compressor(6, 1),
		engage(1),
		release(2),
		roller_Encoder(DIGITAL_SLOT, 5, DIGITAL_SLOT, 6),
		roller(DIGITAL_SLOT, 3),
		time(),
		kicker_state(STATE_IDLE)

	{
		compressor.Start();
		roller_Encoder.SetDistancePerPulse( (2.0 * M_PI * ROLLER_RADIUS) / 1440.0 );
		time.Start();
		roller.Set(1.0);
	}

	//Kick method controls kicker and roller. Activate_kicker parameter is needed for telling
	//kicker when user or robot needs to engage the kicker.
	void Kick()
	{		
		//mutex thing here
		
		if (kicker_state == STATE_IDLE && roller_Encoder.GetRate() <= ROLLER_SPEED)
			kicker_state = STATE_START_KICK;
	}
	
private:
	static void TimerFn(void * param)
	{ 
		((Kicker*)param)->KickTimerFn(); 
	}
	
	void Start()
	{
		m_notifier.StartPeriodic(.0025);
	}
	
	void KickTimerFn()
	{
		switch(kicker_state)
		{

		case STATE_DELAY_KICKER:
			roller.Set(0.0);
			time.Reset();
			kicker_state = STATE_START_KICK;
			break;	
		case STATE_START_KICK:
			if(time.Get() > ROLLER_STOP_TIME)
			{
				roller.Set(0.0);
				engage.Set(true);
				release.Set(false);
				time.Reset();
				kicker_state = STATE_RELEASE;
			}
			break;
		case STATE_RELEASE:
			if(time.Get() > KICK_TIME)
			{
				roller.Set(-1.0);
				engage.Set(false);
				release.Set(true);
				time.Reset();
				kicker_state = STATE_RESET;
			}
			break;
		case STATE_RESET:
			if(time.Get() < KICK_TIME)
				break;
			
			kicker_state = STATE_IDLE;
			
		case STATE_IDLE:
			roller.Set(-1.0);
			engage.Set(false);
			release.Set(false);
			break;
		}
	}

	enum{ STATE_DELAY_KICKER, STATE_IDLE, STATE_START_KICK, STATE_RELEASE, STATE_RESET};
	
	const static double KICK_TIME = .5;
	const static double ROLLER_STOP_TIME = .1;
	const static double ROLLER_RADIUS = 1.0;
	const static double ROLLER_SPEED = .5;
	RobotResources& m_resources;
	Notifier m_notifier;
	Compressor compressor;
	Solenoid engage;
	Solenoid release;
	Encoder roller_Encoder;
	Jaguar roller;
	Timer time;
	int kicker_state;
	

};

#endif

