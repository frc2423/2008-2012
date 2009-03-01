/**
	\file 		RobotMain.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include "WPILib.h"

#include "Framework/KwarqsDriveController.h"
#include "Framework/DriverStationLCD.h"

#include "ServoCalibrator.h"

#include "Controls/SimpleControl.h"
#include "Controls/CompassDrive.h"
#include "Controls/NullMovementControl.h"
#include "Controls/CompassDriveII.h"

#include "Autonomous/AutonomousDemo.h"
#include "Autonomous/AutonomousRandomDemo.h"

#include "Framework/KwarqsGamePiece.h"
#include "Framework/KwarqsBCDInput.h"

#include "PsuedoGearbox.h"
#include "SwerveDrive.h"

class KwarqsRobotMain : public SimpleRobot
{
	DriverStationLCD *		lcd;
	KwarqsDriveController 	driveController;
	
	RobotChassis			chassis;
	
	ServoCalibrator			servoCalibrator;
	
	// control types
	SimpleControl 			simpleControl;
	CompassDrive			compassDrive;
	CompassDriveII			compassDriveII;
	NullMovementControl		nullMovementControl;
	
	AutonomousDemo			autonomousDemo;
	AutonomousRandomDemo	autonomousRandomDemo;
	
	KwarqsGamePiece 		gamePiece;
	
	// drive types
	
	// filters
	PsuedoGearbox			psuedoGearbox;
	
	// motor drivers
	SwerveDrive				swerveDrive;
	
	KwarqsMovementControl * currentTeleoperatedControl;
	
	KwarqsJoystick			m_stick;

public:

	/**
		\brief Constructor for the main robot control class

		To get things to happen, you need to add your class initialization
		and startup code here. 
		
		This executes as soon as the code loads
	*/
	KwarqsRobotMain() :
		ds(DriverStation::GetInstance()),
		servoCalibrator(&chassis),
		simpleControl(&driveController),
		compassDrive(&driveController),
		compassDriveII(&driveController),
		
		nullMovementControl(&driveController),
		
		autonomousDemo(&driveController),
		autonomousRandomDemo(&driveController),
		
		swerveDrive(&chassis),
		currentTeleoperatedControl(NULL),
		m_stick(1)
	{
		GetWatchdog().SetExpiration(200);
		
		// add drive types to the drive controller
		
		// note: they must be added in the order that they should 
		// be called in -- anything that touches the motors should
		// be called last
		
		// todo: need a better way to do this, not satisfied with this
		//driveController.AddDrive(&speedLimiter, DriveEnabled);
		driveController.AddDrive(&psuedoGearbox, DriveEnabled);
		driveController.AddDrive(&swerveDrive, DriveEnabled);
		
		lcd = DriverStationLCD::GetInstance();
	}

	/**
		\brief Selects an autonomous mode
		
		@todo implement autonomous modes
	*/
	KwarqsMovementControl * GetAutonomousMovementControl()
	{
		return &autonomousDemo;
		
		/*
		switch (GetBCDInput())
		{
			case 1:
				
				
			case 2:
				return &autonomousRandomDemo;
				
			default:
				return &nullMovementControl;
		}
		*/
	}
	
	
	/**
		\brief Selects a user control type
		
		This will read switches on the operator control interface and 
		determine which control type to use.
	*/
	KwarqsMovementControl * GetTeleoperatedMovementControl()
	{	
		// select the type (todo: need to read switches)
		KwarqsMovementControl * control;
		
		switch (GetBCDInput())
		{
			case 7:
				control = &compassDriveII;
				break;
		
			case 8:
				control = &compassDrive;
				break;
				
			case 9: 
				control = &simpleControl;
				break;
			
			default:
				control = &nullMovementControl;
		}
		
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
		
		lcd->PrintfLine(DriverStationLCD::kMain_Line6, "%.1f %s", 
				PositionInformation::GetInstance()->GetNormalizedAngle(), control->Name());
		
		return currentTeleoperatedControl;
	}
	
	
	
	/**
		\brief This function is executed during autonomous mode
		
		Should not need to be modified.
	*/
	void Autonomous()
	{
		GetWatchdog().SetEnabled(false);
		psuedoGearbox.Enable();
		
	
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
		double update_time = GetTime();
		
		psuedoGearbox.Enable();
		
		printf("Entered OperatorControl()\n");
		GetWatchdog().SetEnabled(true);
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if (ds->GetDigitalIn(CALIBRATION_SWITCH))
				servoCalibrator.Calibrate();
			else
			{
				//if (m_stick.GetRawButton(5) && m_stick.GetRawButton(6))
				//	PositionInformation::GetInstance()->ResetHeading();
				
				GetTeleoperatedMovementControl()->Move();
				driveController.EndMove();
				
				gamePiece.PerformMovement();
			}
			
			if (GetTime() - update_time > 0.2)
			{
				// update and clear
				lcd->UpdateLCD();
				lcd->Printf(DriverStationLCD::kUser_Line2, 1, "                   ");
				lcd->Printf(DriverStationLCD::kUser_Line3, 1, "                   ");
				lcd->Printf(DriverStationLCD::kUser_Line4, 1, "                   ");
				lcd->Printf(DriverStationLCD::kUser_Line5, 1, "                   ");
				lcd->Printf(DriverStationLCD::kUser_Line6, 1, "                   ");
				
				update_time = GetTime();
			}
		}
		
		GetTeleoperatedMovementControl()->OnDisable();
		currentTeleoperatedControl = NULL;
	}
};

START_ROBOT_CLASS(KwarqsRobotMain);

