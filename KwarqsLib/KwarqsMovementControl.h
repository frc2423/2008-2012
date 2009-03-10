/**
	\file 		KwarqsMovementControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef KWARQSLIB_KWARQS_MOVEMENTCONTROL_H
#define KWARQSLIB_KWARQS_MOVEMENTCONTROL_H

#include "KwarqsDriveController.h"

/**
	\class KwarqsMovementControl
	\brief Base class for anything that interacts with the user to move the robot
	
	This class is used exclusively for driving, and does not deal with moving 
	the	game pieces.
	
	Input: If the derived class needs to interact with a joystick or other
	input device, it should create the appropriate objects and interact with 
	them. 
	
	Output: This should only use the KwarqsDriveBase object to move the robot.
*/
class KwarqsMovementControl {
public:

	/**
		Constructor. Derived classes may have other parameters passed to them
		if needed. 
	*/
	KwarqsMovementControl( KwarqsDriveController * driveController );
	
	
	/// \name Methods that need to be overriden by derived classes
	/// @{
	
	/// This is called repeatedly in OperatorControl mode, this method
	/// should use the m_driveController object to move the Robot
	virtual void Move() = 0;
	
	/// This returns the name of the class (useful for generic logging)
	virtual const char * Name() = 0;
	
	/// called when this control is enabled (optional)
	virtual void OnEnable();
	
	/// called when this control is disabled (optional)
	virtual void OnDisable();
	
	/// @}
	
	// virtual destructor
	virtual ~KwarqsMovementControl(){}
	
protected:

	//! This variable is used to move the robot around. Assume it is not NULL
	KwarqsDriveController * m_driveController;

private:
	// enforce the usage of the constructor
	KwarqsMovementControl();
	
};

#endif
