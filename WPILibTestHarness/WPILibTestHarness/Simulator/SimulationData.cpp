/*
    WPILib Test Harness
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License v3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <WPILib/DigitalInput.h>
#include <WPILib/DigitalOutput.h>
#include <WPILib/Encoder.h>
#include <WPILib/Gyro.h>
#include <WPILib/Notifier.h>
#include <WPILib/PWM.h>
#include <WPILib/AnalogChannel.h>

#include "SimulationData.h"


void EncoderInfo::Transfer()
{
	encoder->m_count = count;
}

void GyroInfo::Transfer()
{
	gyro->m_angle = angle;
}

void DigitalIOData::Transfer()
{
	if (digitalInput)
		digitalInput->m_value = value;
	else if (digitalOutput)
		value = digitalOutput->m_value;
}

void PWMData::Transfer()
{
	if (pwm) speed = pwm->m_speed;
}

void RelayData::Transfer()
{

}

void AnalogIOData::Transfer()
{
	if (analogChannel)
		analogChannel->m_value = value;
}
