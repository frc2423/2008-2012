/**
	\file 		RobotMain.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include "WPILib.h"

#include "Framework/KwarqsDriveController.h"
#include "Framework/KwarqsDSLCDStatus.h"

#include "ServoCalibrator.h"

#include "SimpleControl.h"
#include "NullMovementControl.h"


#include "SwerveDrive.h"

class KwarqsRobotMain : public SimpleRobot
{
	DriverStation * 		ds;
	KwarqsDriveController 	driveController;
	//KwarqsDSLCDStatus		status;
	
	RobotChassis			chassis;
	
	ServoCalibrator			servoCalibrator;
	
	// control types
	SimpleControl 			simpleControl;
	NullMovementControl		nullMovementControl;
	
	// drive types
	
	// filters
	//SpeedLimiter			speedLimiter;
	
	// motor drivers
	SwerveDrive				swerveDrive;
	
	KwarqsMovementControl * currentTeleoperatedControl;

public:

	/**
		\brief Constructor for the main robot control class

		To get things to happen, you need to add your class initialization
		and startup code here. 
	*/
	KwarqsRobotMain() :
		ds(DriverStation::GetInstance()),
		servoCalibrator(&chassis),
		simpleControl(&driveController),
		nullMovementControl(&driveController),
		swerveDrive(&chassis),
		currentTeleoperatedControl(NULL)
	{
		GetWatchdog().SetExpiration(200);
		
		// add drive types to the drive controller
		
		// note: they must be added in the order that they should 
		// be called in -- anything that touches the motors should
		// be called last
		
		// todo: need a better way to do this, not satisfied with this
		//driveController.AddDrive(&speedLimiter, DriveEnabled);
		driveController.AddDrive(&swerveDrive, DriveEnabled);
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
		KwarqsMovementControl * control = &simpleControl;
		
		
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
			
			if (ds->GetDigitalIn(CALIBRATION_SWITCH))
				servoCalibrator.Calibrate();
			else
			{
				GetTeleoperatedMovementControl()->Move();
				driveController.EndMove();
			}
		}
		
		GetTeleoperatedMovementControl()->OnDisable();
		currentTeleoperatedControl = NULL;
	}
};

START_ROBOT_CLASS(KwarqsRobotMain);

