/**
	\file 		DelayTime.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-01 01:00:41 -0500 (Sun, 01 Mar 2009) $
	\version 	$Rev: 244 $
*/


#ifndef KWARQSLIB_DELAY_TIME_H
#define KWARQSLIB_DELAY_TIME_H

/**
	\class DelayTime
	\brief Simple class to abstract all that delay stuff away so we don't
	have to keep making silly variables to do it for us
*/
class DelayTime {
public:

	DelayTime(double interval = 0.2) :
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
