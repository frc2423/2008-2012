/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef PWM_H_
#define PWM_H_

#include "SensorBase.h"
#include <Simulator/Simulator.h>

/**
 * Class implements the PWM generation in the FPGA.
 * 
 * The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They are mapped
 * to the hardware dependent values, in this case 0-255 for the FPGA.
 * Changes are immediately sent to the FPGA, and the update occurs at the next
 * FPGA cycle. There is no delay.
 * 
 * As of revision 0.1.10 of the FPGA, the FPGA interprets the 0-255 values as follows:
 *   - 255 = full "forward"
 *   - 254 to 129 = linear scaling from "full forward" to "center"
 *   - 128 = center value
 *   - 127 to 2 = linear scaling from "center" to "full reverse"
 *   - 1 = full "reverse"
 *   - 0 = disabled (i.e. PWM output is held low)
 */
class PWM : public SensorBase
{
public:
	typedef enum {kPeriodMultiplier_1X = 1, kPeriodMultiplier_2X = 2, kPeriodMultiplier_4X = 4} PeriodMultiplier;

	explicit PWM(UINT32 channel) 
	{ 
		m_speed = 0; 
		Simulator::GetInstance()->AddPWM(this, SensorBase::GetDefaultDigitalModule(), channel);
	}
	
	PWM(UINT32 slot, UINT32 channel) 
	{ 
		m_speed = 0; 
		Simulator::GetInstance()->AddPWM(this, slot, channel);
	}
	
	virtual ~PWM() {
		if (Simulator::GetInstance())
			Simulator::GetInstance()->DeletePWM(this);
	}

	void SetPeriodMultiplier(PeriodMultiplier mult);
	void EnableDeadbandElimination(bool eliminateDeadband);
	void SetBounds(INT32 max, INT32 deadbandMax, INT32 center, INT32 deadbandMin, INT32 min);

protected:
	
	void SetSpeed(float speed) { 
		if (speed < -1.0F)
			m_speed = -1.0F;
		else if (speed > 1.0F)
			m_speed = 1.0F;
		else
			m_speed = speed; 
	}
	
	float GetSpeed() { return m_speed; }
	
public:

	float m_speed;

};

#endif
