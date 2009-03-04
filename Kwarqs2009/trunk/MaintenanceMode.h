/**
	\file 		MaintenanceMode.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-01 04:12:01 -0500 (Sun, 01 Mar 2009) $
	\version 	$Rev: 251 $
*/


#ifndef MAINTENANCE_MODE_H
#define MAINTENANCE_MODE_H

#include "KwarqsLib/DelayEvent.h"
#include "KwarqsLib/KwarqsJoystick.h"

#include "KwarqsLib/PositionInformation.h"
#include "KwarqsLib/DriverStationLCD.h"

#include "RobotChassis.h"
#include "ServoCalibrator.h"

class MaintenanceMode {
public:

	MaintenanceMode(RobotChassis * chassis);

	void DoMaintenance(int user_selection);
	
	void Reset();

private:

	void ServoSpeedTest();
	void WheelSpeedTest();
	
	DriverStationLCD * 		m_lcd;
	PositionInformation * 	m_info;

	KwarqsJoystick		m_stick;
	ServoCalibrator 	servoCalibrator;
	
	RobotChassis * 		m_chassis;

	DelayEvent 			m_lcdEvent;
	DelayEvent 			m_triggerEvent;

	int 				m_helpScreen;

};

#endif

