
#ifndef FAKE_PID_CONTROLLER_H
#define FAKE_PID_CONTROLLER_H

#include "math.h"

/// continuous version of P controller
class FakePController {
public:

	FakePController(double p, double min, double max) : 
		m_P(p),
		last_sample_time(0),
		m_maximum(max),
		m_minimum(min),
		m_result(0)
	{}
	
	void SetP(double p)
	{
		m_P = p;
	}
	
	double GetCalculation(double setpoint, double input)
	{
		if (GetTime() - last_sample_time > 0.05)
		{
			double error = setpoint - input;

			if (fabs(error) > (m_maximum - m_minimum)/2)
			{
				if (error > 0)
					error = error  - m_maximum + m_minimum;
				else
					error = error + m_maximum - m_minimum;
			}
					
			m_result = m_P * error;
			
			if (m_result > m_maximum)
				m_result = m_maximum;
			else if (m_result < m_minimum)
				m_result = m_minimum;
				
			last_sample_time = GetTime();
		}
		
		return m_result;
	}
	
	
private:

	FakePController();

	double m_P;
	
	double last_sample_time;
	double last_value;
	
	double m_tolerance;
	
	double m_maximum;
	double m_minimum;
	
	double m_result;
};

#endif

