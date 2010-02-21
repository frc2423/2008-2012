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
		roller_Encoder(DIGITAL_SLOT, 7, DIGITAL_SLOT, 8),
		roller(3),
		time(),
		kicker_state(STATE_IDLE),
		roller_on(true)

	{
		compressor.Start();
		roller_Encoder.SetDistancePerPulse( (2.0 * M_PI * ROLLER_RADIUS) / 1440.0 );
		time.Start();
		roller.Set(1.0);
		
		// create a mutex to allow us to synchronize access to variables
		// since we're going to be running in multiple threads
		m_mutex = semMCreate(0);
		m_displayRollerVoltage = m_resources.webdma.CreateDoubleProxy("Roller", "Roller sensor voltage",
			DoubleProxyFlags().readonly());
	}

	
//Returns true if roller is on, false if not.
	bool GetRoller()
	{
		return roller_on;
	}
	
//Returns true if ball is captured, false if not.
	bool HasBall()
	{
		if(m_resources.ballSensor.GetVoltage() >= BALLSENSOR_VOLTAGE)
			return true;
		else
			return false;
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
			else if( HasBall())
				kicker_state = STATE_START_KICK;
		}
	}
	
	void Start()
	{
		m_notifier.StartPeriodic(.0025);
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
		
		m_displayRollerVoltage = m_resources.ballSensor.GetVoltage();
		
		//Digital Input 2 turns roller on/off
		
		switch(kicker_state)
		{

		case STATE_DELAY_KICKER:
			//setRoller(false);
			roller.Set(1.0);
			time.Reset();
			kicker_state = STATE_START_KICK;
			break;	
		case STATE_START_KICK:
			if(time.Get() > ROLLER_STOP_TIME)
			{
				setRoller(false);
				engage.Set(true);
				release.Set(false);
				time.Reset();
				kicker_state = STATE_RELEASE;
			}
			break;
		case STATE_RELEASE:
			if(time.Get() > KICK_TIME)
			{
				setRoller(true);
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
			//Digital Input 2 turns roller on/off
			if(DriverStation::GetInstance()->GetDigitalIn(ROLLER_DIGITAL_SWITCH))
				setRoller(true);
			else
				setRoller(false);
			engage.Set(false);
			release.Set(false);
			break;
		}
	}
	

//Turns roller on and off
	
	void setRoller(bool on)
	{	
		if(on)
			roller.Set(-1.0);
		else
			roller.Set(0.0);
	}
	
	

	enum{ STATE_DELAY_KICKER, STATE_IDLE, STATE_START_KICK, STATE_RELEASE, STATE_RESET};
	
	const static double KICK_TIME = .5;
	const static double ROLLER_STOP_TIME = 1.0;
	const static double ROLLER_RADIUS = 1.0;
	const static double BALLSENSOR_VOLTAGE = 1.0;
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
	int kicker_state;
	bool roller_on;
	DoubleProxy m_displayRollerVoltage;
	

};

#endif

