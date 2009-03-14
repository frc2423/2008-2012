/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "DigitalInput.h"
#include "Resource.h"
#include "Utility.h"
#include "WPIStatus.h"

static Resource *quadEncoders = NULL;

/**
 * Gets the current count.
 * Returns the current count on the Encoder.
 * This method compensates for the decoding type.
 * 
 * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale factor.
 */
INT32 Encoder::Get()
{
	return (INT32) (GetRaw() * DecodingScaleFactor());
}


/**
 * The scale needed to convert a raw counter value into a number of encoder pulses.
 */
double Encoder::DecodingScaleFactor()
{
	switch (m_encodingType)
	{
	case k1X:
		return 1.0;
	case k2X:
		return 0.5;
	case k4X:
		return 0.25;
	default:
		//TODO: fill this in
		return 0.0;
	}	
}

/**
 * Get the distance the robot has driven since the last reset.
 * 
 * @return The distance driven since the last reset as scaled by the value from SetDistancePerPulse().
 */
double Encoder::GetDistance()
{
	return Get() * m_distancePerPulse;
}

/**
 * Get the current rate of the encoder.
 * Units are distance per second as scaled by the value from SetDistancePerPulse().
 * 
 * @return The current rate of the encoder.
 */
double Encoder::GetRate()
{
	return m_distancePerPulse / GetPeriod() * (GetDirection() ? 1.0 : -1.0);
}

/**
 * Set the minimum rate of the device before the hardware reports it stopped.
 * 
 * @param minRate The minimum rate.  The units are in distance per second as scaled by the value from SetDistancePerPulse().
 */
void Encoder::SetMinRate(double minRate)
{
	SetMaxPeriod(m_distancePerPulse / minRate);
}

/**
 * Set the distance per pulse for this encoder.
 * This sets the multiplier used to determine the distance driven based on the count value
 * from the encoder.
 * Do not include the decoding type in this scale.  The library already compensates for the decoding type.
 * Set this value based on the encoder's rated Pulses per Revolution and
 * factor in gearing reductions following the encoder shaft.
 * This distance can be in any units you like, linear or angular.
 * 
 * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
 */
void Encoder::SetDistancePerPulse(double distancePerPulse)
{
	m_distancePerPulse = distancePerPulse;
}


