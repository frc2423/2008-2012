

#ifndef COMPASSDRIVE_H
#define COMPASSDRIVE_H

#include "../KwarqsLib/KwarqsConstants.h"
#include "../KwarqsLib/KwarqsMovementControl.h"
#include "../KwarqsLib/PositionInformation.h"
#include "../KwarqsLib/KwarqsJoystick.h"


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
};

#endif
