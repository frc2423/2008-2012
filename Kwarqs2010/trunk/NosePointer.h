/**
	\file 		NosePointer.h
	\author 	Amory Galili/Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef NOSE_POINTER_H
#define NOSE_POINTER_H

#include "ProxiedPIDController.h"
#include "RobotResources.h"


class NosePointer : public PIDOutput, public PIDSource
{
public:
	
	NosePointer(RobotResources &resources, double turnHigh, double turnOn );	
	virtual ~NosePointer(){}
	
	
	// returns the turn rate for a desired angle
	double GetTurnRate( double angleToPoint );
	
	// returns true when we are facing the correct direction
	bool IsPointingCorrectly() const ;
	
	
	void PIDWrite(float output);
	double PIDGet();
		
	
private:

	NosePointer();
	DISALLOW_COPY_AND_ASSIGN(NosePointer);
	
	RobotResources 			&m_resources;
	SEM_ID 					m_mutex;
	
	ProxiedPIDController	m_pid;
	
	BoolProxy				m_use_compass_drive;

	// output from pid controller
	double 					m_pid_output;
	
	// parameters
	double					m_turnHigh;
	double					m_turnOn;
	
	// state
	double					m_last_angle;
};


#endif
