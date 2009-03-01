
#ifndef KWARQS_PIDDEMO_H
#define KWARQS_PIDDEMO_H

#include "PIDController.h"
#include "PIDOutput.h"
#include "PIDSource.h"

#include <math.h>

class PIDDemo : public PIDOutput, public PIDSource {
public:
	
	/**
	 * Allocate a PID object with the given constants for P, I, D
	 * @param Kp the proportional coefficient
	 * @param Ki the integral coefficient
	 * @param Kd the derivative coefficient
	 */
	PIDDemo(float Kp, float Ki, float Kd) :
		m_output(0), m_position(0),
		m_controller(Kp, Ki, Kd, this, this, .25)
	{
        m_controller.SetContinuous();
        m_controller.SetInputRange(0, 360);
        m_controller.SetOutputRange(-360, 360);

		m_controller.Enable();
	}
	
#define SCALE 45.0

	void PIDWrite(float output)
	{
		if (output > SCALE)
			m_output = 1;
		else if (output < -SCALE)
			m_output = -1;
		else
			m_output = output / SCALE;

        m_position += output;

		if (m_position > 360.0)
			m_position -= 360.0;
		else if (m_position < 0)
			m_position += 360.0;
	}
	
	double PIDGet()
	{
		return fmod(m_position, 360.0);
	}
	
	void SetSetPoint(float input)
	{
		m_controller.SetSetpoint(input);
	}

	double GetOutput()
	{
		return m_output;
	}
	void SetPosition(float position)
	{
		m_position = position;
	}
	double GetPosition()
	{
		return m_position;
	}
    bool OnTarget()
    {
        return m_controller.OnTarget();
    }
	
	double 			m_output, m_position;

	PIDController 	m_controller;
};

#endif
