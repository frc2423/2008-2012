/**
	\file 		CompassDriveII.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef COMPASSDRIVE_II_H
#define COMPASSDRIVE_II_H

#include "../KwarqsLib/KwarqsNosePointer.h"
#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/KwarqsMovementControl.h"
#include "../KwarqsLib/PositionInformation.h"
#include "../KwarqsLib/KwarqsJoystick.h"
#include "../KwarqsLib/DelayEvent.h"

/**
	\class CompassDriveII
	\brief A very simple control system to demonstrate that things work
*/
class CompassDriveII : public KwarqsMovementControl {
public:

	// constructor
	CompassDriveII(KwarqsDriveController * driveController);

	/// required method
	void Move();
	
	/// required method
	const char * Name() { return "CompassDriveII"; }
	
private:

	double CalculateSpeed();
	double CalculateWheelDirection();
	double controllerCompass();
	double robotCompass();


	// joystick used for control
	KwarqsJoystick m_stick;
	KwarqsJoystick m_stick2;
	
	PositionInformation * m_position;
	
	KwarqsNosePointer m_nosePointer;
	
	DelayEvent 	m_motorDelay;
	double m_lastSpeed;
};

#endif

