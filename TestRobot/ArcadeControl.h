/**
	\file 		ArcadeControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef ARCADE_CONTROL_H
#define ARCADE_CONTROL_H

#include "Framework/KwarqsConstants.h"
#include "Framework/KwarqsMovementControl.h"


class ArcadeControl : public KwarqsMovementControl {
public:

	// constructor
	ArcadeControl(KwarqsDriveController * driveController);

	// required methods
	void Move();
	const char * Name() { return "ArcadeControl"; }
	
private:

	// joystick used for control
	Joystick * m_stick;
};

#endif

