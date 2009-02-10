/**
	\file 		KwarqsDriveController.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include "WPILib.h"
#include "KwarqsDriveController.h"


void KwarqsDriveController::AddDrive(KwarqsDriveBase * drive, DriveEnabledType enabled)
{
	m_driveTypes.push_back(drive);
	
	if (enabled == DriveEnabled)
	{
		m_enabledDrives.push_back(drive);
		drive->Enable();
	}
}

void KwarqsDriveController::DisableDrive(KwarqsDriveBase * drive)
{
	DriveIterator i = m_enabledDrives.begin();
	const DriveIterator end = m_enabledDrives.end();

	for (; i != end; i++)
		if (*i == drive)
		{
			// found it, get rid of it
			m_enabledDrives.erase(i);
			drive->Disable();
			break;
		}
}

void KwarqsDriveController::EnableDrive(KwarqsDriveBase * drive)
{
	//DriveIterator i = m_enabledDrives.begin();
	//const DriveIterator end = m_enabledDrives.end();

	//for (; i != end; i++)
	
	wpi_assert(0 && "not implemented");
}


void KwarqsDriveController::Move(double speed, double angle, double rotation)
{
	DriveIterator i = m_enabledDrives.begin();
	const DriveIterator end = m_enabledDrives.end();
	
	// call all the drives to drive the bot, allowing the filters to modify
	// the drive parameters if needed
	for (; i != end; i++)
		(*i)->Move(speed, angle, rotation);
	
	m_moved = true;
}


void KwarqsDriveController::EndMove()
{
	// if we haven't moved yet, then its time to do so
	if (!m_moved)
		Move(m_lastSpeed, m_lastAngle, m_lastRotation);
		
	m_moved = false;
}


