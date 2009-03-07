/**
	\file 		MaintenanceMode.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef MAINTENANCE_MODE_H
#define MAINTENANCE_MODE_H

#include "KwarqsLib/DelayEvent.h"
#include "KwarqsLib/KwarqsJoystick.h"

#include "KwarqsLib/PositionInformation.h"
#include "KwarqsLib/DriverStationLCD.h"

#include "RobotChassis.h"
#include "ServoCalibrator.h"

#include <vector>

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

	bool				print_to_lcd;
	DelayEvent 			m_printEvent;
	DelayEvent 			m_triggerEvent;

	int 				m_helpScreen;

	int					last_user_selection;
	
	// servo test
	DelayEvent			m_servoEvent;
	int					m_servoTestRunning;
	
	std::vector<INT32>	m_lf_servo_vals;
	std::vector<INT32>	m_lr_servo_vals;
	std::vector<INT32>	m_rf_servo_vals;
	std::vector<INT32>	m_rr_servo_vals;
	
	// motor test
	DelayEvent			m_motorEvent;

};

#endif

