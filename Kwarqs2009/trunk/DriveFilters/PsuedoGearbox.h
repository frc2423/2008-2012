/**
	\file 		PsuedoGearbox.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef PSEUDOGEARBOX_H
#define PSEUDOGEARBOX_H

#include "../WebInterface/WebInterface.h"

/**
	\class PsuedoGearbox
	\brief This is a trivially simple filter drive class that limits the 
	speed of the robot. There is a question of whether this belongs here 
	or not. I feel.. no. Oh well. 
*/
class PsuedoGearbox : public KwarqsDriveBase 
{
public:

	PsuedoGearbox() :
		m_stick(FIRST_JOYSTICK_PORT),
		m_controller(CONTROLLER_JOYSTICK_PORT),
		m_enabled(true),
		m_speed_pct(WebInterface::CreateDoubleProxy("Limiter", "Speed %",
				DoubleProxyFlags().minval(0).maxval(1).default_value(.5).step(.1))),
		m_rotation_pct(WebInterface::CreateDoubleProxy("Limiter", "Rotation %",
				DoubleProxyFlags().minval(0).maxval(1).default_value(.5).step(.1)))
	{}

	void Disable(){
		m_enabled = false;
	}
	void Enable(){
		m_enabled = true;
	}

	/// this is a filter, so return false
	virtual bool IsPhysicalDrive() { return false; }

	/// limits the speed of the robot
	virtual void Move(double &speed, double &angle, double &rotation, bool &stop)
	{
		// don't touch this (turbo mode)
		if (!m_enabled || m_stick.GetTrigger() || 
			m_controller.GetRawButton(2) || m_controller.GetRawButton(6))
			return;
	
		// low speed action (boring, but makes robot easier to drive)
		speed *= m_speed_pct;
		rotation *= m_rotation_pct;
	}

	/// Return the name of the class
	virtual const char * Name() { return "PsuedoGearbox"; }
	
private:

	KwarqsJoystick m_stick;
	KwarqsJoystick m_controller;

	bool m_enabled;
	
	DoubleProxy m_speed_pct;
	DoubleProxy m_rotation_pct;
};

#endif
