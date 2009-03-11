/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef QUAD_ENCODER_H_
#define QUAD_ENCODER_H_

#include <VxWorks.h>
#include "CounterBase.h"
#include "PIDSource.h"

#include <Simulator/Simulator.h>

class DigitalSource;

/**
 * Class to read quad encoders.
 * Quadrature encoders are devices that count shaft rotation and can sense direction. The output of
 * the QuadEncoder class is an integer that can count either up or down, and can go negative for
 * reverse direction counting. When creating QuadEncoders, a direction is supplied that changes the
 * sense of the output to make code more readable if the encoder is mounted such that forward movement
 * generates negative values. Quadrature encoders have two digital outputs, an A Channel and a B Channel
 * that are out of phase with each other to allow the FPGA to do direction sensing. 
 */
class Encoder: public CounterBase
{
public:
	Encoder(UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel, bool reverseDirection=false, EncodingType encodingType = k4X)
	{
		m_started = false;
		m_count = 0;
		m_reverseDirection = reverseDirection;
		m_encodingType = encodingType;
		
		Simulator::GetInstance()->AddEncoder(this, aSlot, aChannel, bSlot, _bChannel);
	}
	
	virtual ~Encoder()
	{
		if (Simulator::GetInstance())
			Simulator::GetInstance()->DeleteEncoder(this);
	}

	// CounterBase interface
	void Start() { m_started = true; }
	INT32 Get();
	INT32 GetRaw() { return m_count; }
	void Reset() { m_count = 0; }
	void Stop() { m_started = false; }
	
	double GetPeriod() { return m_period; }
	void SetMaxPeriod(double maxPeriod){}
	bool GetStopped(){ assert(0 && "not implemented"); return false; }
	bool GetDirection() { return m_direction; }
	double GetDistance();
	double GetRate();
	void SetMinRate(double minRate);
	void SetDistancePerPulse(double distancePerPulse);
	void SetReverseDirection(bool reverseDirection)
	{
		m_reverseDirection = reverseDirection;
	}

	INT32 m_count;
	double m_period;
	bool m_direction;		// true if positive increment, false if negative
	
private:

	double DecodingScaleFactor();

	EncodingType m_encodingType;
	bool m_started;
	bool m_reverseDirection;
	double m_distancePerPulse;
};

#endif

