/**
	\file 		NullMovementControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef NULLMOVEMENTCONTROL_H
#define NULLMOVEMENTCONTROL_H

#include "../KwarqsLib/KwarqsMovementControl.h"

class NullMovementControl : public KwarqsMovementControl {
public:

	NullMovementControl(KwarqsDriveController * driveController) :
		KwarqsMovementControl(driveController)
	{}

	void Move() 
	{
		// stay where you're at
		m_driveController->Move(0,0,0, false);
	}
	
	/// This returns the name of the class (useful for generic logging)
	const char * Name() { return "NullMovement"; }
};

#endif
