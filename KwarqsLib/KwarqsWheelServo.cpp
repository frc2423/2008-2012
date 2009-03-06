/**
	\file 		KwarqsWheelServo.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/
/*
 * Copyright (c) 2009 Dustin Spicuzza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Dustin Spicuzza nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DUSTIN SPICUZZA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DUSTIN SPICUZZA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <WPILib.h>

#include <math.h>
#include "KwarqsWheelServo.h"

/**
	\brief Constructor
	
	There are far too many parameters to remember, so for ease
	of use you can look at KwarqsConstants.h for some macros
	that define the appropriate parameters. 
	
	@param slot					Slot for electronics
	@param pwm_port				PWM port for motor
	@param encoder_port1		Encoder port
	@param encoder_port2		Encoder port 
	@param cal_port				Calibration port
	@param cal_offset			Calibration offset (ie, when it goes low what 
								angle are we currently facing?)
	@param outputScale			A value used to indicate the max error angle to 
								generate a full-power correctional response
	@param encoderResolution	The number of wheel counts that it takes for the wheel
								to rotate 360 degrees
	@param invert_motor			Set to true to invert the speed of the motor (it is assumed
								that positive moves to the left)
	@param invert_encoder		Set to true to invert the output of the encoder
	@param param_p				The P parameter of the PID loop
	
*/
KwarqsWheelServo::KwarqsWheelServo(
	UINT32 slot,
	UINT32 pwm_port, 
	UINT32 encoder_port1, UINT32 encoder_port2,
	UINT32 cal_port,
	double cal_offset,
	double outputScale,
	int encoderResolution,
	bool invert_motor,
	bool invert_encoder,
	float param_p
) :
	m_motor(slot, pwm_port),
	m_encoder(slot, encoder_port1, slot, encoder_port2, invert_encoder),
	m_sensor(slot, cal_port),
	m_outputScale(outputScale),
	m_setAngle(0),
	m_encoderResolution(encoderResolution),
	m_calibrating(false),
	m_calibrating_offset(cal_offset),
	m_invert_motor( invert_motor ? -1.0F : 1.0F )
	
{
	// initialize the calibration mutex
	m_calibration_mutex = semBCreate(0, SEM_FULL);

	// no calibration by default
	CalibrationComplete();
		
	// create the PID controller
	m_pidController = new PIDController(param_p, 0.0F, 0.0F, this, this);
	
	// set the PID parameters
	m_pidController->SetContinuous();
	m_pidController->SetInputRange(0, 360);
	m_pidController->SetOutputRange(-360, 360);
	m_pidController->SetTolerance(0.0025F);
	
	// enable the servo by default
	m_encoder.Start();
	m_pidController->Enable();
}

KwarqsWheelServo::~KwarqsWheelServo()
{
	delete m_pidController;
}

/// Enables the servo
void KwarqsWheelServo::Enable()
{
	m_motor.Set(0);
	m_pidController->Enable();
}

/// Disables the servo
void KwarqsWheelServo::Disable()
{
	m_pidController->Disable();
	m_motor.Set(0);
}

/// Sets the current position as 0 degrees
void KwarqsWheelServo::Reset()
{
	CalibrationComplete();
}

void KwarqsWheelServo::EnableManual()
{
	m_pidController->Disable();
}

void KwarqsWheelServo::AutoCalibrate()
{
#ifdef AUTOCALIBRATE_SERVO

	Synchronized sync(m_calibration_mutex);
	
	// obviously, don't do this twice
	if (m_calibrating)
		return;
	
	m_calibrating = true;
	
	// setup the interrupt handlers
	m_sensor.RequestInterrupts(&KwarqsWheelServo::CalibrationIrqHandler, this);
		
	m_sensor.SetUpSourceEdge(false, true);
	m_sensor.EnableInterrupts();
#endif
}

void KwarqsWheelServo::CalibrationComplete()
{
	// no multithreading errors... 
	Synchronized sync(m_calibration_mutex);

#ifdef AUTOCALIBRATE_SERVO
	if (m_calibrating)
		m_sensor.DisableInterrupts();
#endif

	m_calibrated_offset = m_encoder.GetRaw() % m_encoderResolution;	
	m_calibrating = false;
}


/// Get the angle that the wheel is supposed to be pointing
double KwarqsWheelServo::GetSetAngle()
{
	return m_setAngle;
}


/// Get the angle that the wheel is actually pointing
double KwarqsWheelServo::GetCurrentAngle()
{
	// convert the encoder value to an angle
	double angle = fmod((((double)((m_encoder.GetRaw() - m_calibrated_offset) % m_encoderResolution) * 360.0) 
			/ (double)m_encoderResolution) - m_calibrating_offset, 360.0);
			
	if (angle < 0)
		angle += 360;
		
	return angle;
}


/// Returns true if the calibration sensor is active, false otherwise
bool KwarqsWheelServo::GetSensor()
{
	return !m_sensor.Get();
}


/// Set the angle that the wheel should be pointing, where
/// 0 is straight ahead and angle increments positively 
/// counter clockwise
void KwarqsWheelServo::SetAngle(double angle)
{
	// make sure you handle negative angles correctly.. otherwise
	// we will get bad oscillations
	angle = fmod(angle, 360.0);
	if (angle < 0)
		angle += 360.0; 
	
	m_setAngle = angle;
	
	m_pidController->SetSetpoint((float)angle);
}


// generally you won't need to use this.. 
void KwarqsWheelServo::TuneParameters(float p, float i, float d)
{
	m_pidController->SetPID(p, i, d);
}


/// called when the PID Controller completes a calculation
void KwarqsWheelServo::PIDWrite(float output)
{
	// calibration mode
#ifdef AUTOCALIBRATE_SERVO
	{
		Synchronized sync(m_calibration_mutex);
		if (m_calibrating)
		{
			m_motor.Set(m_invert_motor);
			return;
		}
	}
#endif
	
	// the value received should be in the range of -360, 360 so we should
	// just scale it to a value the victor can take.. but then we would not
	// have a good response time (probably). So, we use the scale parameter
	// to determine the rate of change in relation to the output
	
	// don't need to check the value for > 1 -- its done in WPILib
	output = output/(float)m_outputScale;
	
	// set the motor value
	
	if (OnTarget())
		m_motor.Set(0);
	else
		m_motor.Set(output * m_invert_motor);
}

double KwarqsWheelServo::PIDGet()
{
	return GetCurrentAngle();
}


bool KwarqsWheelServo::OnTarget()
{
	bool temp;
	temp = (fabs(m_setAngle - GetCurrentAngle())< 0.0025 / 100 * 720);

	return temp;
}



#ifdef AUTOCALIBRATE_SERVO

/// IRQ handler for the calibration sensor
void KwarqsWheelServo::CalibrationIrqHandler(tNIRIO_u32 x, void * param)
{
	((KwarqsWheelServo *)param)->CalibrationComplete();
}

#endif

