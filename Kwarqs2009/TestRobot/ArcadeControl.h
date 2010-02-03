/**
	\file 		ArcadeControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef ARCADE_CONTROL_H
#define ARCADE_CONTROL_H

#include "KwarqsLib/KwarqsConstants.h"
#include "KwarqsLib/KwarqsMovementControl.h"
#include "KwarqsLib/KwarqsJoystick.h"


class ArcadeControl : public KwarqsMovementControl {
public:

	// constructor
	ArcadeControl(KwarqsDriveController * driveController);

	// required methods
	void Move();
	const char * Name() { return "ArcadeControl"; }
	
private:

	// joystick used for control
	KwarqsJoystick m_stick;
};

#endif

