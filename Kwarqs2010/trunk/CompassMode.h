#ifndef COMPASSMODE_H
#define COMPASSMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"

class CompassMode : public RobotMode
{
public:
	CompassMode(RobotResources& resources);

	void OnEnable();
	void Main();
	void OnDisable();
	
	double speed();
	double turnRate();




private:
	RobotResources& m_resources;
	
	const static float turn_const = 1.0;
};





#endif

