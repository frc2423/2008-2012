/**
	\file 		SimpleControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef ARCADE_CONTROL_H
#define ARCADE_CONTROL_H

#include "Framework/KwarqsDevices.h"
#include "Framework/KwarqsMovementControl.h"


class SimpleControl : public KwarqsMovementControl {
public:

	// constructor
	SimpleControl(KwarqsDriveController * driveController);

	// required methods
	void Move();
	const char * Name() { return "SimpleControl"; }
	
private:

	// joystick used for control
	Joystick * m_stick;
	
	Gyro gyro;
};

#endif

