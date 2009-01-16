
#ifndef KWARQS_PIDDEMO_H
#define KWARQS_PIDDEMO_H

#include "PIDController.h"
#include "PIDOutput.h"
#include "PIDSource.h"


class PIDDemo : public PIDOutput, public PIDSource {
public:
	
	/**
	 * Allocate a PID object with the given constants for P, I, D
	 * @param Kp the proportional coefficient
	 * @param Ki the integral coefficient
	 * @param Kd the derivative coefficient
	 */
	PIDDemo(float Kp, float Ki, float Kd) :
		m_output(0),
		m_controller(Kp, Ki, Kd, .25)
	{
		m_controller.SetInput(this, 0, 2);
		m_controller.SetOutput(this, 0, 2);
		m_controller.Enable();
	}
	
	void PIDWrite(float output)
	{
		m_output = output;
	}
	
	float PIDGet()
	{
		return m_output;
	}
	
	void SetSetPoint(float input)
	{
		m_controller.SetSetpoint(input);
	}

	float GetOutput()
	{
		return m_output;
	}
	
	float 			m_output;
	PIDController 	m_controller;
};

#endif
