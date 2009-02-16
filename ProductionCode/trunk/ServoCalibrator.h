
#ifndef SERVOCALIBRATOR_H
#define SERVOCALIBRATOR_H

#include "RobotChassis.h"

class ServoCalibrator {
public:

	ServoCalibrator(RobotChassis * chassis);
	void Calibrate();

private:

	Joystick * m_stick;
	RobotChassis * m_chassis;
	
};

#endif
