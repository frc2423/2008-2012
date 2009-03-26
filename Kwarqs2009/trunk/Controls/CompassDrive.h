/**
	\file 		CompassDrive.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef COMPASSDRIVE_H
#define COMPASSDRIVE_H

#include "../KwarqsLib/KwarqsNosePointer.h"
#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/KwarqsMovementControl.h"
#include "../KwarqsLib/PositionInformation.h"
#include "../KwarqsLib/KwarqsJoystick.h"
#include "../KwarqsLib/DelayEvent.h"


class CompassDrive : public KwarqsMovementControl {
public:

	// constructor
	CompassDrive(KwarqsDriveController * driveController);

	/// required method
	void Move();
	
	/// required method
	const char * Name() { return "CompassDrive"; }
	
private:

	double CalculateSpeed();
	double CalculateWheelDirection();
	double controllerCompass();
	double robotCompass();


	// joystick used for control
	KwarqsJoystick m_stick;
	PositionInformation * m_position;
	
	DelayEvent m_twistEvent;
	
	KwarqsNosePointer m_nosePointer;
	double m_noseAngle;
};

#endif
