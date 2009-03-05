/**
	\file 		RobotMain.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>

#include "KwarqsLib/KwarqsDriveController.h"
#include "KwarqsLib/DriverStationLCD.h"
#include "KwarqsLib/KwarqsBCDInput.h"
#include "KwarqsLib/DelayEvent.h"
#include "KwarqsLib/KwarqsTelemetry.h"
#include "KwarqsLib/PositionInformation.h"

#include "MaintenanceMode.h"

#include "Autonomous/AutonomousDemo.h"
#include "Autonomous/AutonomousRandomDemo.h"

#include "Controls/AnnControl.h"
#include "Controls/SimpleControl.h"
#include "Controls/CompassDrive.h"
#include "Controls/NullMovementControl.h"
#include "Controls/CompassDriveII.h"



#include "DriveFilters/PsuedoGearbox.h"

#include "Drives/AnnDrive.h"
#include "Drives/SwerveDrive.h"

#include "KwarqsGameControl.h"

#include "TelemetryData.h"



class KwarqsRobotMain : public SimpleRobot
{
	DriverStationLCD *		lcd;
	KwarqsDriveController 	driveController;
	
	RobotChassis			chassis;
	
	MaintenanceMode			maintenanceMode;
	
	// control types
	AnnControl				annControl;
	SimpleControl 			simpleControl;
	CompassDrive			compassDrive;
	CompassDriveII			compassDriveII;
	NullMovementControl		nullMovementControl;
	
	AutonomousDemo			autonomousDemo;
	AutonomousRandomDemo	autonomousRandomDemo;
	
	KwarqsGameControl 		gameControl;
	
	// drive types
	
	// filters
	PsuedoGearbox			psuedoGearbox;
	
	// motor drivers
	SwerveDrive				swerveDrive;
	AnnDrive				annDrive;
	
	KwarqsMovementControl * currentTeleoperatedControl;
	
	KwarqsJoystick			m_stick;
	
	DelayEvent				m_lcdUpdateEvent;
	DelayEvent				m_telemetryEvent;
	
	KwarqsTelemetry<TelemetryData> m_telemetry;
	
	PositionInformation *	m_info;

public:

	/**
		\brief Constructor for the main robot control class

		To get things to happen, you need to add your class initialization
		and startup code here. 
		
		This executes as soon as the code loads
	*/
	KwarqsRobotMain() :
		maintenanceMode(&chassis),
		
		annControl(&driveController),
		simpleControl(&driveController),
		compassDrive(&driveController),
		compassDriveII(&driveController),
		
		nullMovementControl(&driveController),
		
		autonomousDemo(&driveController),
		autonomousRandomDemo(&driveController),
		
		swerveDrive(&chassis),
		annDrive(&chassis),
		
		currentTeleoperatedControl(NULL),
		m_stick(1),
		
		m_lcdUpdateEvent(0.3),
		m_telemetryEvent(0.05),
		
		m_info(PositionInformation::GetInstance())
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
		
		/// @todo read from a file, figure out which autonomous we want
		/// to use from looking at that
	
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
	KwarqsMovementControl * GetTeleoperatedMovementControl(int user_selection)
	{	
		// select the type that the user wants
		KwarqsMovementControl * control;
		
		switch (user_selection)
		{
			case 6:
				control = &annControl;
				break;
		
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
		
		lcd->PrintfLine(DriverStationLCD::kMain_Line, "%s %.1f", 
				control->Name(), PositionInformation::GetInstance()->GetNormalizedFieldAngle());
		
		return currentTeleoperatedControl;
	}
	
	
	
	/**
		\brief This function is executed during autonomous mode
		
		Should not need to be modified.
	*/
	void Autonomous()
	{
		GetWatchdog().SetEnabled(false);
		
		gameControl.TurnoffMotors();
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
			
			// update telemetry
			UpdateTelemetry();
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
		bool last_mode = false;
		int user_selection = 0;
		
		psuedoGearbox.Enable();
		
		printf("Entered OperatorControl()\n");
		GetWatchdog().SetEnabled(true);
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			// get the user selection
			user_selection = GetBCDInput();
			bool maintenance_mode = m_ds->GetDigitalIn(MAINTENANCE_MODE_SWITCH);
			
			if (last_mode != maintenance_mode)
			{
				maintenanceMode.Reset();
				last_mode = maintenance_mode;
			}
			
			if (maintenance_mode)
			{
				gameControl.TurnoffMotors();
				maintenanceMode.DoMaintenance(user_selection);
			}
			else
			{			
				// do the users preferred movement here
				GetTeleoperatedMovementControl(user_selection)->Move();
				driveController.EndMove();
				
				// and the game control
				gameControl.PerformMovement();
				
				// update telemetry
				UpdateTelemetry();	
			}
			
			// allow the user to reset gyro position if needed
			if (m_stick.GetRawButton(5) && m_stick.GetRawButton(6))
				PositionInformation::GetInstance()->ResetHeading();
			
			UpdateLCD();			
		}
		
		GetTeleoperatedMovementControl(user_selection)->OnDisable();
		currentTeleoperatedControl = NULL;
	}
	
	/// updates the lcd output
	void UpdateLCD()
	{
		if (m_lcdUpdateEvent.DoEvent())
		{
			// update and clear
			lcd->UpdateLCD();
			lcd->Clear();
		}
	}
	
	void UpdateTelemetry()
	{
		if (m_telemetryEvent.DoEvent())
			m_telemetry.AddData( TelemetryData(chassis, m_info) );
	}
	
};

START_ROBOT_CLASS(KwarqsRobotMain);

