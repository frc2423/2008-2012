/**
	\file 		CompassMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef COMPASSDRIVE_H
#define COMPASSDRIVE_H

#include <WPILib.h>
#include "RobotResources.h"


class CompassDrive
{
public:
	CompassDrive(RobotResources& resources);
	
	double speed();
	double turnRate(float turnHigh, float turnOn);


private:
	RobotResources& m_resources;

};


#endif

