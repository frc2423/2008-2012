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


template <int N>
KwarqsWheelServo * UglyServoHack<N>::that = NULL;



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
	
	@param outputScale			A value used to indicate the max error angle to 
								generate a full-power correctional response
	@param encoderResolution	The number of wheel counts that it takes for the wheel
								to rotate 360 degrees
	@param cal_offset			Calibration offset (ie, when it goes low what 
								angle are we currently facing?)
	
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
	bool invert_encoder
) :
	m_motor(slot, pwm_port),
	m_encoder(slot, encoder_port1, slot, encoder_port2, invert_encoder),
	m_sensor(slot, cal_port),
	m_outputScale(outputScale),
	m_encoderResolution(encoderResolution),
	m_calibrating(false),
	m_calibrating_offset(cal_offset),
	m_invert_motor( invert_motor ? -1 : 1 )
{
	// no calibration by default
	CalibrationComplete();
		

	// create the PID controller
	m_pidController = new TunablePIDController(0.95, 0, 0, this, this);
	
	// set the PID parameters
	m_pidController->SetContinuous();
	m_pidController->SetInputRange(0, 360);
	m_pidController->SetOutputRange(-360, 360);
	m_pidController->SetTolerance(0.0025);
	
	// enable it
	m_pidController->Enable();
	m_encoder.Start();
}

KwarqsWheelServo::~KwarqsWheelServo()
{
	delete m_pidController;
}

void KwarqsWheelServo::Enable()
{
	m_pidController->Enable();
}

void KwarqsWheelServo::Disable()
{
	m_pidController->Disable();
}

void KwarqsWheelServo::Calibrate()
{
	/*
		disable this -- it no longer works

	// don't do this twice
	if (m_calibrating)
		return;
	
	m_calibrating = true;
	KwarqsWheelServo::m_uncalibrated_servos++;
	
	// setup the interrupt handlers.. this is very braindead, why
	// can't I just pass the *this* pointer in?
	
	tInterruptHandler handler;
	
	SETUP_UGLY_SERVO_HACK
	
	m_sensor.RequestInterrupts(handler);
		
	m_sensor.SetUpSourceEdge(false, true);
	m_sensor.EnableInterrupts();
	
	*/
}

void KwarqsWheelServo::CalibrationComplete()
{
	/*
	if (m_calibrating && !m_in_manual_mode)
	{
		KwarqsWheelServo::m_uncalibrated_servos--;
		m_sensor.DisableInterrupts();
	}
	*/

	m_calibrated_offset = m_encoder.GetRaw() % m_encoderResolution;	
	m_calibrating = false;
}

void KwarqsWheelServo::EnableManualCalibration()
{
	m_calibrating = true;
	m_in_manual_mode = true;
}

void KwarqsWheelServo::DisableManualCalibration()
{
	if (m_in_manual_mode)
	{
		m_in_manual_mode = false;
		CalibrationComplete();
	}
}



/// Set the angle that the wheel should be pointing, where
/// 0 is straight ahead and angle increments positively 
/// counter clockwise
void KwarqsWheelServo::SetAngle(double angle)
{
	// make sure you handle negative angles correctly.. otherwise
	// we will enter bad oscillations
	angle = fmod(angle, 360.0);
	if (angle < 0)
		angle += 360.0;
	m_pidController->SetSetpoint(angle);
}

/// Get the angle that the wheel is supposed to be pointing
double KwarqsWheelServo::GetSetAngle()
{
	return m_pidController->GetSetpoint();
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

bool KwarqsWheelServo::GetSensor()
{
	return !m_sensor.Get();
}

// generally you won't need to use this.. 
void KwarqsWheelServo::TuneParameters(float p, float i, float d)
{
	m_pidController->TuneParameters(p, i, d);
}


/// called when the PID Controller completes a calculation
void KwarqsWheelServo::PIDWrite(float output)
{
	// calibration mode
	/*
	if (m_calibrating)
	{
		if (m_in_manual_mode)
		{
			Joystick * stick = Joystick::GetStickForPort(1);
			
			// power of 3 so its easier to calibrate the motor
			m_motor.Set( pow(stick->GetY() * -1 * m_invert_motor, 3));
		}
		else
			m_motor.Set(m_invert_motor);
		return;
	}
	*/
	
	// the value received should be in the range of -360, 360 so we should
	// just scale it to a value the victor can take.. but then we would not
	// have a good response time (probably). So, we use the scale parameter
	// to determine the rate of change in relation to the output
	
	// don't need to check the value for > 1 -- its done in WPILib
	output = output/m_outputScale;
	
	// set the motor value
	if (m_pidController->OnTarget())
		m_motor.Set(0);
	else
		m_motor.Set(output * m_invert_motor);
}

double KwarqsWheelServo::PIDGet()
{
	return GetCurrentAngle();
}

/** static functions **/

int KwarqsWheelServo::m_uncalibrated_servos = 0;

int KwarqsWheelServo::UncalibratedServoCount()
{
	return KwarqsWheelServo::m_uncalibrated_servos;
}
