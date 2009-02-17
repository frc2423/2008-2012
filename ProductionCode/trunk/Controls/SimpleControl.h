/**
	\file 		SimpleControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef ARCADE_CONTROL_H
#define ARCADE_CONTROL_H

#include "../Framework/KwarqsConstants.h"
#include "../Framework/KwarqsMovementControl.h"

/**
	\class SimpleControl
	\brief A very simple control system to demonstrate that things work
*/
class SimpleControl : public KwarqsMovementControl {
public:

	// constructor
	SimpleControl(KwarqsDriveController * driveController);

	/// required method
	void Move();
	
	/// required method
	const char * Name() { return "SimpleControl"; }
	
private:

	// joystick used for control
	Joystick * m_stick;
};

#endif

