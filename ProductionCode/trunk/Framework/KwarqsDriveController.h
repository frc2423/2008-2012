/**
	\file 		KwarqsDriveController.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef FRAMEWORK_KWARQS_DRIVECONTROLLER_H
#define FRAMEWORK_KWARQS_DRIVECONTROLLER_H

#include <vector>

enum DriveEnabledType {
	DriveEnabled,
	DriveDisabled
};

class KwarqsDriveController {
public:

	KwarqsDriveController() :
		m_lastSpeed(0), m_lastRotation(0),
		m_moved(false)
	{}
	
	/// Initialization: adds a drive type to the list of drives
	void AddDrive(KwarqsDriveBase * drive, DriveEnabledType enabled);
	
	/// disable a drive type
	void DisableDrive(KwarqsDriveBase * drive);
	
	/// enable a drive type
	void EnableDrive(KwarqsDriveBase * drive);
	
	/// this is called when a KwarqsMovementController wants to move the bot
	void Move(float speed, float rotation);
	
	/// this is called after all moving is complete for this iteration
	/// of the loop is done -- only used by KwarqsRobotMain
	void EndMove();
	
private:

	typedef std::vector<KwarqsDrive *> 		DriveContainer;
	typedef DriveContainer::iterator		DriveIterator;

	// all drive types
	DriveContainer 							m_driveTypes;
	
	// all drives that are enabled
	DriveContainer 							m_enabledDrives;

	// ensures that move is called for every iteration of the
	// loop for algorithms that require that
	float 	m_lastSpeed, m_lastRotation;
	bool 	m_moved;
	
	
};

#endif
