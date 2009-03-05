/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include "Base.h"
#include "Utility.h"

typedef void (*TimerEventHandler)(void *param);

class Notifier
{
public:
	Notifier(TimerEventHandler handler, void *param) :
		m_handler(handler), m_param(param)
	{
		Simulator::AddNotifier(this);
	}
	
	virtual ~Notifier()
	{
		Simulator::DeleteNotifier(this);
	}
	
	void StartSingle(double period){
		m_enabled = true;
		m_single = true;
		m_period = period;
		m_next_event = GetTime() + period;
	}
	void StartPeriodic(double period)
	{
		m_enabled = true;
		m_single = false;
		m_period = period;
		m_next_event = GetTime() + period;
	}
	
	void Stop()
	{
		m_enabled = false;
	}
	
	
	// simulation interface
	void TryEvent(double tm)
	{
		if (m_enabled && m_next_event > tm)
		{
			(*m_handler)(m_param);
		
			if (m_single)
				m_enabled = false;
			else
				m_next_event = tm + m_period;
		}
	}
	
private:
	DISALLOW_COPY_AND_ASSIGN(Notifier);
	
	TimerEventHandler m_handler;
	void * m_param;
	
	bool m_enabled, m_single;
	double m_period;
	
	double m_next_event;
};

#endif
