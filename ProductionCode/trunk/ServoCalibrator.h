/**
	\file 		ServoCalibrator.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-02-16 09:05:51 -0500 (Mon, 16 Feb 2009) $
	\version 	$Rev: 166 $
*/

#ifndef SERVOCALIBRATOR_H
#define SERVOCALIBRATOR_H

#define POT_ARRAY_SIZE 10

#include "RobotChassis.h"

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
