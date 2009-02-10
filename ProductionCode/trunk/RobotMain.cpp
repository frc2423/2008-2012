/**
	\file 		RobotMain.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include "WPILib.h"

#include "Framework/KwarqsDriveController.h"

#include "ArcadeControl.h"
#include "NullMovementControl.h"

#include "SpeedLimiter.h"

#include "ArcadeDrive.h"

class KwarqsRobotMain : public SimpleRobot
{
	KwarqsDriveController 	driveController;
	
	// control types
	ArcadeControl 			arcadeControl;
	NullMovementControl		nullMovementControl;
	
	// drive types
	
	// filters
	SpeedLimiter			speedLimiter;
	
	// motor drivers
	// XXX: Remember that if we have more than one set of motor 
	// drivers, they have to share PWM's .. worry about it later
	ArcadeDrive				arcadeDrive;
	
	KwarqsMovementControl * currentTeleoperatedControl;

public:

	/**
		\brief Constructor for the main robot control class

		To get things to happen, you need to add your class initialization
		and startup code here. 
	*/
	KwarqsRobotMain() :
		arcadeControl(&driveController),
		nullMovementControl(&driveController),
		currentTeleoperatedControl(NULL)
	{
		GetWatchdog().SetExpiration(200);
		
		// add drive types to the drive controller
		
		// note: they must be added in the order that they should 
		// be called in -- anything that touches the motors should
		// be called last
		
		// todo: need a better way to do this, not satisfied with this
		//driveController.AddDrive(&speedLimiter, DriveEnabled);
		driveController.AddDrive(&arcadeDrive, DriveEnabled);
	}

	/**
		\brief Selects an autonomous mode
		
		@todo implement autonomous modes
	*/
	KwarqsMovementControl * GetAutonomousMovementControl()
	{
		return &nullMovementControl;
	}
	
	
	/**
		\brief Selects a user control type
		
		This will read switches on the operator control interface and 
		determine which control type to use.
	*/
	KwarqsMovementControl * GetTeleoperatedMovementControl()
	{	
		// select the type (todo: need to read switches)
		KwarqsMovementControl * control = &arcadeControl;
		
		
		// enable or disable it depending on whether it was previously
		// selected
		if (currentTeleoperatedControl != control)
		{
			// if its newly disabled, then call the appropriate function
			if (currentTeleoperatedControl)
				currentTeleoperatedControl->OnDisable();
			
			currentTeleoperatedControl = control;
			currentTeleoperatedControl->OnEnable();
		}
		
		return currentTeleoperatedControl;
	}
	
	
	
	/**
		\brief This function is executed during autonomous mode
		
		Should not need to be modified.
	*/
	void Autonomous()
	{
		GetWatchdog().SetEnabled(false);
		
		// this only gets selected at the beginning of autonomous mode,
		// it doesn't make any sense to allow it to change in the middle!
		KwarqsMovementControl * movementControl = GetAutonomousMovementControl();
		
		// enable the control
		movementControl->OnEnable();
		
		while (IsAutonomous())
		{
			movementControl->Move();
			driveController.EndMove();
		}
		
		// disable it
		movementControl->OnDisable();
	}

	/**
		\brief This function is executed during Teleoperated mode
		
		Should not need to be modified
	*/
	void OperatorControl()
	{
		printf("Entered OperatorControl()\n");
		GetWatchdog().SetEnabled(true);
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			GetTeleoperatedMovementControl()->Move();
			driveController.EndMove();
			
		}
		
		GetTeleoperatedMovementControl()->OnDisable();
		currentTeleoperatedControl = NULL;
	}
};

START_ROBOT_CLASS(KwarqsRobotMain);

