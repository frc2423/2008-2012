/**
	\file 		KwarqsGameControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef KWARQSGAMECONTROL_H
#define KWARQSGAMECONTROL_H

/**
	\class KwarqsGameControl
	\brief This encapsulates all of the game piece control
*/
class KwarqsGameControl {
public:

	KwarqsGameControl();
	
	/// Turns the drive motors off -- necessary for autonomous mode
	void TurnoffMotors();
	
	/// called in operatorControl -- this reads any user input and performs
	/// the necessary actions needed
	void PerformMovement();
	
private:
	
	Victor m_low_arm_motor;
	Victor m_upper_arm_motor;
	Joystick * m_stick;

};

#endif
