
#ifndef _NOTIFIER_H
#define _NOTIFIER_H

typedef void (*TimerEventHandler)(void *param);

class Notifier {
public:

	Notifier(TimerEventHandler handler, void * arg)
	{
		Notifier::m_handler = handler;
		Notifier::m_handler_arg = arg;
	}

	void StartPeriodic(double period)
	{}

	static void Pulse()
	{
		Notifier::m_handler(m_handler_arg);
	}

	static TimerEventHandler m_handler;
	static void * m_handler_arg;

};

#endif



