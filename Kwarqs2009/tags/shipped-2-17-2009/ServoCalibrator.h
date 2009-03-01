/**
	\file 		ServoCalibrator.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef SERVOCALIBRATOR_H
#define SERVOCALIBRATOR_H

#define POT_ARRAY_SIZE 10

#include "RobotChassis.h"
#include "Framework/KwarqsJoystick.h"

class ServoCalibrator {
public:

	ServoCalibrator(RobotChassis * chassis);
	void Calibrate();

private:
	
	bool ConvertStickToAngle(double &angle);
	double GetFilteredPotValue();

	KwarqsJoystick m_stick, m_motorStick;
	RobotChassis * m_chassis;
	double m_time;
	
	bool m_in_pot_mode;
	
	//double m_pot_array[POT_ARRAY_SIZE];
	//int m_idx;
	//double m_pot_time;
	
};

#endif
