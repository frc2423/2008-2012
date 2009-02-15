/**
	\file 		KwarqsDSLCDStatus.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-02-10 02:52:53 -0500 (Tue, 10 Feb 2009) $
	\version 	$Rev: 122 $
*/

#ifndef FRAMEWORK_KWARQS_DSLCD_STATUS_H
#define FRAMEWORK_KWARQS_DSLCD_STATUS_H

#include "Notifier.h"
#include "DriverStationLCD.h"

#include "KwarqsWheelServo.h"

/**
	\class KwarqsDSLCDStatus
	\brief Outputs various status to the driver station LCD
	
	This is done on another thread so we don't have to worry
	about annoying things. You just create this object and
	it does stuff.
	
	@todo: add easy way to add monitored variables or something
	to that effect
*/
class KwarqsDSLCDStatus {
public:
	
	KwarqsDSLCDStatus();
	~KwarqsDSLCDStatus();
	
private:
	
	// this function does the update
	static void OnInterval(void *);
	
	Notifier * m_notifier; 
};

#endif
