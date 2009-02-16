
#ifndef SERVOCALIBRATOR_H
#define SERVOCALIBRATOR_H

#include "RobotChassis.h"

class ServoCalibrator {
public:

	ServoCalibrator(RobotChassis * chassis);
	void Calibrate();

private:
	
	bool ConvertStickToAngle(double &angle);

	Joystick * m_stick, * m_motorStick;
	RobotChassis * m_chassis;
	double m_time;
	
};

#endif
