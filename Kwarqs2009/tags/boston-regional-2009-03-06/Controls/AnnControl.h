/**
	\file 		AnnControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef ANNCONTROL_H
#define ANNCONTROL_H

#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/KwarqsMovementControl.h"
#include "../KwarqsLib/KwarqsJoystick.h"

/**
	\class AnnControl
	\brief A very simple control system to demonstrate that things work
*/
class AnnControl : public KwarqsMovementControl {
public:

	// constructor
	AnnControl(KwarqsDriveController * driveController);

	/// required method
	void Move();
	
	/// required method
	const char * Name() { return "AnnControl"; }
	
private:

	// joystick used for control
	KwarqsJoystick m_stick;
};

#endif

