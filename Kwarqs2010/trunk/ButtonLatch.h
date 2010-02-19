
#ifndef BUTTON_LATCH_H
#define BUTTON_LATCH_H


/**
 * usage:
 * 
 * ButtonLatch x;
 * 
 * x.Set( some_button_value );
 * 
 * if (x.TurnedOn())
 * {
 * 		// do first action here
 * }
 * else if (x.TurnedOff())
 * {
 * 		// do action on turned off
 * }
 * 
 */
class ButtonLatch {
public:
	
	ButtonLatch( bool initial = false ) :
		m_last_value(initial),
		m_turned_on(false),
		m_turned_off(false)
	{}
	
	void Set( bool value )
	{
		if (m_last_value != value )
		{
			if (value)
				m_turned_on = true;
			else
				m_turned_off = true;
			
			m_last_value = value;
		}
		
	}
	
	bool TurnedOn()
	{
		bool retval = m_turned_on;
		m_turned_on = false;
		return retval;
	}
	
	bool TurnedOff()
	{
		bool retval = m_turned_off;
		m_turned_off = false;
		return retval;
	}
	
	bool On() const
	{
		return m_last_value;
	}
	
	bool Off() const
	{
		return !m_last_value;
	}
	
	
private:
	
	bool m_last_value;
	
	bool m_turned_on;
	bool m_turned_off;
	
};


#endif
