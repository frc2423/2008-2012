/**
	\file 		CompassController.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef COMPASSCONTROLLER_H
#define COMPASSCONTROLLER_H

#include "../KwarqsLib/KwarqsNosePointer.h"
#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/KwarqsMovementControl.h"
#include "../KwarqsLib/PositionInformation.h"
#include "../KwarqsLib/KwarqsJoystick.h"
#include "../KwarqsLib/DelayEvent.h"

#include "../WebInterface/WebInterface.h"

/**
	\class CompassController
	\brief A very simple control system to demonstrate that things work
*/
class CompassController : public KwarqsMovementControl {
public:

	// constructor
	CompassController(KwarqsDriveController * driveController);

	/// required method
	void Move();
	
	/// required method
	const char * Name() { return "CompassController"; }
	
private:

	double CalculateSpeed();
	double CalculateWheelDirection();
	double controllerCompass();
	double robotCompass();


	KwarqsJoystick m_controller;
	
	PositionInformation * m_position;
	
	KwarqsNosePointer m_nosePointer;
	
	DelayEvent m_spinEvent;
	double m_noseDirection;
	
	DoubleProxy m_bigButtonAngle;
	DoubleProxy m_smallButtonAngle;
};

#endif

