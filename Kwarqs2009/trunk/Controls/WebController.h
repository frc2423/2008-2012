/**
	\file 		WebController.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-04-27 16:22:07 -0400 (Mon, 27 Apr 2009) $
	\version 	$Rev: 369 $
*/


#ifndef WebController_H
#define WebController_H

#include "../KwarqsLib/KwarqsNosePointer.h"
#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/KwarqsMovementControl.h"
#include "../KwarqsLib/PositionInformation.h"
#include "../KwarqsLib/KwarqsJoystick.h"
#include "../KwarqsLib/DelayEvent.h"

#include "../WebInterface/WebInterface.h"

/**
	\class WebController
	\brief A very simple control system to demonstrate control via Web
*/
class WebController : public KwarqsMovementControl {
public:

	// constructor
	WebController(KwarqsDriveController * driveController);

	/// required method
	void Move();
	
	/// required method
	const char * Name() { return "WebController"; }
	
	bool IsEnabled() const { return m_enabled; }
	
private:

	PositionInformation * m_position;
	
	KwarqsNosePointer m_nosePointer;
	
	BoolProxy 		m_enabled;
	DoubleProxy 	m_angle;
	DoubleProxy 	m_heading;
	DoubleProxy 	m_speed;
	BoolProxy 		m_stop;
};

#endif

