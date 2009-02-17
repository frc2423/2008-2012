

#ifndef COMPASSDRIVE_H
#define COMPASSDRIVE_H

#include "../Framework/KwarqsConstants.h"
#include "../Framework/KwarqsMovementControl.h"
#include "../Framework/PositionInformation.h"


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
	Joystick * m_stick;
	PositionInformation * m_position;
};

#endif
