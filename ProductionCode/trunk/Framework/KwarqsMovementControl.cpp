/**
	\file 		KwarqsMovementControl.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include "KwarqsMovementControl.h"

// constructor
KwarqsMovementControl::KwarqsMovementControl(
	KwarqsDriveController * driveController
) :
	m_driveController(driveController);
{}

