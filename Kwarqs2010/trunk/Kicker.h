/**
	\file 		Kicker.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef KICKER_H
#define KICKER_H

#include <WPILib.h>
#include "RobotResources.h"

#include "filters.h"


class Kicker
{
	

	enum RollerState { ROLLER_STATE_ON, ROLLER_STATE_OFF, ROLLER_STATE_ACTION };
	
public:
	Kicker(RobotResources& resources):
		kicker_state(STATE_IDLE),
		m_resources(resources),
		m_notifier(Kicker::TimerFn, this),
		compressor(6, 1),
		engage(1),
		release(2),
		roller_Encoder(DIGITAL_SLOT, 7, DIGITAL_SLOT, 8),
		roller(3),
		ballInput(10)
	{
		compressor.Start();
		roller_Encoder.SetDistancePerPulse( (2.0 * M_PI * ROLLER_RADIUS) / 1440.0 );
		time.Start();
		
		// create a mutex to allow us to synchronize access to variables
		// since we're going to be running in multiple threads
		m_mutex = semMCreate(0);
		m_displayRollerVoltage = m_resources.webdma.CreateDoubleProxy("Kicker", "Roller sensor voltage",
			DoubleProxyFlags().readonly());
		
		m_hasBall = m_resources.webdma.CreateBoolProxy("Kicker", "Has Ball", false );
		
		m_ballSwitch = m_resources.webdma.CreateBoolProxy("Kicker", "Ball Switch", false );
			
		
		m_rollerActionTime = m_resources.webdma.CreateDoubleProxy("Kicker", "Kicker Action Time",
					DoubleProxyFlags()
						.default_value(0.00)
						.minval(0.0)
						.maxval(2.0)
						.step(0.01)
				);
		
		m_rollerActionSpeed = m_resources.webdma.CreateDoubleProxy("Kicker", "Kicker Action Speed",
					DoubleProxyFlags()
						.default_value(1)
						.minval(-1.0)
						.maxval(1.0)
						.step(0.1)
				);
	}
	
	void Start()
	{
		m_notifier.StartPeriodic(.0025);
	}

	
	//Returns true if ball is captured, false if not.
	bool HasBall()
	{
		return m_hasBall;
	}
	

	/* Kick method used to engage kicker:
	 * 
	 * -If digital switch 1 on the Driverstation is off, kicker is activated regardless
	 * 	of roller speed.
	 * -If digital switch 1 on the Driverstation is on, kicker is activated only if the
	 * 	roller is not spinning.
	*/	
	void Kick()
	{		
		// make sure that nobody else can access our variables
		Synchronized lock(m_mutex);
			
		if (kicker_state == STATE_IDLE)
		{
			if(!DriverStation::GetInstance()->GetDigitalIn(KICKER_DIGITAL_SWITCH)) 
				kicker_state = STATE_START_KICK;
			else if( HasBall() )
				kicker_state = STATE_START_KICK;
				
			// ok, we're starting a kick, get ready for it
			if (kicker_state == STATE_START_KICK)
			{
				// kicks always start by shutting down the roller
				SetRoller( ROLLER_STATE_ACTION );
				time.Reset();
			}
		}
	}
	
	// returns True if in the process of kicking, false otherwise
	bool IsKicking()
	{
		Synchronized lock(m_mutex);
		
		return kicker_state != STATE_IDLE;
	}

	
	
private:
	static void TimerFn(void * param)
	{ 
		((Kicker*)param)->KickTimerFn(); 
	}
	
	void KickTimerFn()
	{
		// make sure that nobody else can access our variables
		Synchronized lock(m_mutex);
		
		//roller_filter.AddPoint( m_resources.ballSensor.GetVoltage() );
		
		//m_displayRollerVoltage = roller_filter.GetAverage();
		
		m_ballSwitch = ballInput.Get();
		
		if(!m_ballSwitch && kicker_state == STATE_IDLE)
			m_hasBall = true;
		else
			m_hasBall = false;
		

		switch(kicker_state)
		{
					
		case STATE_START_KICK:
		
			// this state engages the pnuematics to perform the kick
			// appropriately after a short delay
		
			if(time.Get() > m_rollerActionTime)
			{				
				engage.Set(true);
				release.Set(false);
				
				time.Reset();
				kicker_state = STATE_RELEASE;
			}
			
			break;
			
		case STATE_RELEASE:
		
			// this state waits for the pnuematics to fully release
			// before bringing them back
		
			if(time.Get() > KICK_TIME)
			{
				SetRoller( ROLLER_STATE_ON );				
				engage.Set(false);
				release.Set(true);
				
				time.Reset();
				kicker_state = STATE_RESET;
			}
			
			break;
			
		case STATE_RESET:
		
			// this state waits for the pneumatics to come back to their
			// starting position and then we fall back to idle mode
		
			if(time.Get() < KICK_TIME)
				break;
			
			kicker_state = STATE_IDLE;
			// fall through to the next state
			
		case STATE_IDLE:
			
			// this is the normal operation of the kicker mechanism
			
			SetRoller( ROLLER_STATE_ON );			
			engage.Set(false);
			release.Set(false);
			
			break;
		}
	}
	

	//Turns roller on and off
	void SetRoller(RollerState state)
	{	
		// some specified input controls whether roller is normally on/off
		bool roller_enabled = DriverStation::GetInstance()->GetDigitalIn(ROLLER_DIGITAL_SWITCH);		
		
		if (!roller_enabled)
		{
			roller.Set(0.0);
			return;
		}
		
		switch (state)
		{
		case ROLLER_STATE_ON:
			roller.Set(-1.0);
			break;
		case ROLLER_STATE_OFF:
			roller.Set(0.0);
			break;
		case ROLLER_STATE_ACTION:
			roller.Set(m_rollerActionSpeed);
			break;
		}
	}
	
	
	enum{ STATE_IDLE, STATE_START_KICK, STATE_RELEASE, STATE_RESET } kicker_state;
	
	const static double KICK_TIME = .5; 
	const static double ROLLER_RADIUS = 1.0;
	const static double BALLSENSOR_VOLTAGE = 0.7;
	const static int KICKER_DIGITAL_SWITCH = 1;
	const static int ROLLER_DIGITAL_SWITCH = 2;
	
	RobotResources& m_resources;
	Notifier m_notifier;
	Compressor compressor;
	Solenoid engage;
	Solenoid release;
	Encoder roller_Encoder;
	Jaguar roller;
	Timer time;
	SEM_ID m_mutex;
	
	DoubleProxy m_rollerActionSpeed;
	DoubleProxy m_rollerActionTime;
	DoubleProxy m_displayRollerVoltage;
	BoolProxy	m_hasBall;
	BoolProxy 	m_ballSwitch;
	
	DigitalInput ballInput;
	
	//AverageWindowFilter<double, 10> roller_filter;
	

};

#endif

