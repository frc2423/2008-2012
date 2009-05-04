/**
	\file 		ServoCalibrator.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef SERVOCALIBRATOR_H
#define SERVOCALIBRATOR_H


#include "RobotChassis.h"
#include "KwarqsLib/KwarqsJoystick.h"
#include "KwarqsLib/DriverStationLCD.h"
#include "KwarqsLib/DelayEvent.h"

#include "KwarqsLib/filters.h"

#include "WebInterface/WebInterface.h"

class ServoCalibrator {
public:

	ServoCalibrator(RobotChassis * chassis);
	
	void DoAutoCalibrate();
	void DoManualCalibrate();
	
	void DoLFServo();
	void DoLRServo();
	void DoRFServo();
	void DoRRServo();
	
	void Reset();

private:
	
	void DoServo(KwarqsWheelServo &servo);

	void ShowLCDOutput();

	KwarqsJoystick 		m_stick;
	RobotChassis * 		m_chassis;
	DriverStation * 	m_ds;
	DriverStationLCD * 	m_lcd;

	DelayEvent m_lcdEvent;
	DelayEvent m_triggerEvent;
	
	AverageWindowFilter<double, 20> m_potfilter1;
	AverageWindowFilter<double, 20> m_potfilter2;
	AverageWindowFilter<double, 20> m_potfilter3;
	AverageWindowFilter<double, 20> m_potfilter4;
	
	double pot1offset;
	double pot2offset;
	double pot3offset;
	double pot4offset;
	
	DoubleProxy proxy_lf;
	DoubleProxy proxy_lr;
	DoubleProxy proxy_rf;
	DoubleProxy proxy_rr;
	
	DoubleProxy proxy_mlf;
	DoubleProxy proxy_mlr;
	DoubleProxy proxy_mrf;
	DoubleProxy proxy_mrr;
	
	bool m_begin_manual_calibrate;
	
	bool last_trig;
};

#endif
