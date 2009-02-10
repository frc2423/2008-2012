/**
	\file 		KwarqsDriveController.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef FRAMEWORK_KWARQS_DRIVECONTROLLER_H
#define FRAMEWORK_KWARQS_DRIVECONTROLLER_H

#include <vector>
#include "KwarqsDriveBase.h"

enum DriveEnabledType {
	DriveEnabled,
	DriveDisabled
};

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
	 	
	 	@param speed	Speed for the robot to move
	 	@param angle	Angle in degrees for the bot to move, where straight 
	 					ahead is 0 degrees, and directly right is 90 degrees.
	  	@param rotation	Angular rotation speed
	 */
	void Move(double speed, double angle, double rotation);
	
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
	bool 	m_moved;
	
	
};

#endif
