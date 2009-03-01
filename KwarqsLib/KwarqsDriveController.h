/**
	\file 		KwarqsDriveController.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQSLIB_KWARQS_DRIVECONTROLLER_H
#define KWARQSLIB_KWARQS_DRIVECONTROLLER_H

#include <vector>
#include "KwarqsDriveBase.h"

enum DriveEnabledType {
	DriveEnabled,
	DriveDisabled
};

/**
	\brief Used by KwarqsMovementControl derived classes to move the robot
	
	This is the only supported interface for controlling the robot directly,
	as it supports filtering and other useful things. 
*/
class KwarqsDriveController {
public:

	KwarqsDriveController() :
		m_lastSpeed(0), m_lastAngle(0), m_lastRotation(0),
		m_moved(false)
	{}
	
	/// Initialization: adds a drive type to the list of drives
	void AddDrive(KwarqsDriveBase * drive, DriveEnabledType enabled);
	
	/// disable a drive type
	void DisableDrive(KwarqsDriveBase * drive);
	
	/// enable a drive type
	void EnableDrive(KwarqsDriveBase * drive);
	
	/** 
	 	\brief Called when a KwarqsMovementController wants to move the bot
	 	
	 	@param speed	Speed for the robot to move (-1 to 1)
	 	@param angle	Angle in degrees for the bot to move, where straight 
	 					ahead is 0 degrees, and angle increments positively in
						a clockwise direction
	  	@param rotation	Angular rotation (-1 to 1)
	  	@param stop		Stop the robot motion completely
	 */
	void Move(double speed, double angle, double rotation, bool stop);
	
	/// this is called after all moving is complete for this iteration
	/// of the loop is done -- only used by KwarqsRobotMain
	void EndMove();
	
private:

	typedef std::vector<KwarqsDriveBase *> 	DriveContainer;
	typedef DriveContainer::iterator		DriveIterator;

	// all drive types
	DriveContainer 							m_driveTypes;
	
	// all drives that are enabled
	DriveContainer 							m_enabledDrives;

	// ensures that move is called for every iteration of the
	// loop for algorithms that require that
	double 	m_lastSpeed, m_lastAngle, m_lastRotation;
	bool 	m_moved, m_lastStop;
	
	
};

#endif
