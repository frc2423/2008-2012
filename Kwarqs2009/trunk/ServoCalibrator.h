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
#include "KwarqsLib/KwarqsJoystick.h"
#include "KwarqsLib/DriverStationLCD.h"

class ServoCalibrator {
public:

	ServoCalibrator(RobotChassis * chassis);
	
	void DoAutoCalibrate();
	void DoManualCalibrate();

private:

	void ShowLCDOutput();

	KwarqsJoystick m_stick;
	RobotChassis * m_chassis;
	DriverStationLCD * lcd;

	DelayEvent m_lcdDelay;
	DelayEvent m_triggerEvent;
	
	AverageWindowFilter<double, 20> potfilter1;
	AverageWindowFilter<double, 20> potfilter2;
	AverageWindowFilter<double, 20> potfilter3;
	AverageWindowFilter<double, 20> potfilter4;
	
	double pot1offset;
	double pot2offset;
	double pot3offset;
	double pot4offset;
	
	bool m_begin_manual_calibrate;
	
	
};

#endif
