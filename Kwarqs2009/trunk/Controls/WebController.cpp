/**
	\file 		WebController.cpp
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-04-27 16:22:07 -0400 (Mon, 27 Apr 2009) $
	\version 	$Rev: 369 $
*/

#include <WPILib.h>

#include "WebController.h"

#include "../KwarqsLib/math.h"
#include "../KwarqsLib/KwarqsConstants.h"


WebController::WebController(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	m_position(PositionInformation::GetInstance()),
	m_nosePointer("WebController")
{	
	m_enabled = WebInterface::CreateBoolProxy("WebController", "Enabled", false);

	m_speed = WebInterface::CreateDoubleProxy("WebController", "Speed", 
		DoubleProxyFlags().default_value(0).minval(-1).maxval(1).step(.1));
	
	m_heading = WebInterface::CreateDoubleProxy("WebController", "Heading", 
		DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(1));
	
	m_angle = WebInterface::CreateDoubleProxy("WebController", "Nose Angle", 
		DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(1));
		
	m_stop = WebInterface::CreateBoolProxy("WebController", "Stop", false);
}



void WebController::Move()
{	
	double rotation = m_nosePointer.GetRotation(m_angle);
	m_driveController->Move(m_speed, m_position->TranslateFieldToRobotAngle( m_heading ), rotation, m_stop);
}





