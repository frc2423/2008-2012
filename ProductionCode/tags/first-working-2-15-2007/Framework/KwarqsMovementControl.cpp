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
	m_driveController(driveController)
{}


void KwarqsMovementControl::OnEnable()
{
	printf("%s enabled\n", Name());
}

void KwarqsMovementControl::OnDisable()
{
	printf("%s disabled\n", Name());
}

