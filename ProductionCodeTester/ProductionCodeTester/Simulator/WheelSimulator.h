
#ifndef SIMULATOR_WHEEL_SIMULATOR_H
#define SIMULATOR_WHEEL_SIMULATOR_H

#include "SimulationData.h"

class SpeedController;
class Encoder;
class DigitalInput;

class WheelSimulator {
public:

	// pass in necessary stuff
	void Initialize(
		WheelInfo * info, 
		SpeedController * motor,
		Encoder * motorEncoder,
		SpeedController * servo,
		Encoder * servoEncoder,
		DigitalInput * hall_effect);

	void Step();

private:

	WheelInfo * 		m_info;
	
	SpeedController * 	m_motor;
	Encoder * 			m_motorEncoder;
	SpeedController * 	m_servo;
	Encoder * 			m_servoEncoder;
	DigitalInput * 		m_hall_effect;

};

#endif
