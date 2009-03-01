/**
	\file 		KwarqsGamePiece.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef FRAMEWORK_KWARQS_GAMEPIECE_H
#define FRAMEWORK_KWARQS_GAMEPIECE_H

/**
	\class KwarqsGamePiece
	\brief This encapsulates all of the game pieces
	
	This class is not fully defined yet, given we don't know what is
	needed to control the game pieces. However, it should provide necessary
	methods needed to do the actions during autonomous mode
*/
class KwarqsGamePiece {
public:

	KwarqsGamePiece();
	
	/// called in operatorControl -- this reads any user input and performs
	/// the necessary actions needed
	void PerformMovement();
	
private:
	
	Victor m_low_arm_motor;
	Victor m_upper_arm_motor;
	Joystick * m_stick;

};

#endif
