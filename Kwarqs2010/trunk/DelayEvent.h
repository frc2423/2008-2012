/**
	\file 		DelayEvent.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-05 01:27:36 -0500 (Thu, 05 Mar 2009) $
	\version 	$Rev: 272 $
*/


#ifndef KWARQSLIB_DELAY_EVENT_H
#define KWARQSLIB_DELAY_EVENT_H

/**
	\class DelayEvent
	\brief Simple class to abstract delayed actions away
*/
class DelayEvent {
public:

	DelayEvent(double interval = 0.15) :
		m_interval(interval),
		m_lastUpdate(GetTime())
	{}

	/// returns true if the interval has expired
	inline bool DoEvent()
	{
		double current_time = GetTime();
		if (current_time - m_lastUpdate > m_interval)
		{
			m_lastUpdate = current_time;
			return true;
		}
		
		return false;
	}

private:
	double m_interval;
	double m_lastUpdate;
};

#endif
