
#include <WPILib/WPILib.h>
#include "WheelSimulator.h"


// so we assume:

// CIM: 4320 RPM, 60 oz/in torque
// Each wheel rotation should equal 100/7 encoder revolutions (4:1 bevel gear ratio * 50/14 gearbox encoder ratio).

// 

// servo wheel: 72/22 = encoder output
// window: 70RPM
// globe: 100RPM


void WheelSimulator::Initialize(
		WheelInfo * info, 
		SpeedController * motor,
		Encoder * motorEncoder,
		SpeedController * servo,
		Encoder * servoEncoder,
		DigitalInput * hall_effect)
{
	m_info = info;
	m_motor = motor;
	m_motorEncoder = motorEncoder;
	m_servo = servo;
	m_servoEncoder = servoEncoder;
	m_hall_effect = hall_effect;
}
	
	
void WheelSimulator::Step()
{
	m_info->driveMotorSpeed = m_motor->Get();
	m_info->servoMotorSpeed = m_servo->Get();

	// todo: fill in other parameters
}

