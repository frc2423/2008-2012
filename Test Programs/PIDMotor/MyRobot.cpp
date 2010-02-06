#include "WPILib.h"

#include <WebDMA/WebDMA.h>
#include "PIDMotor.h"

#include <limits>
#undef min
#undef max

#define M_PI 3.141592653589793238462643

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	// pid values to use
	#define PID_P 0.5
	#define PID_I 0.0
	#define PID_D 0.0

	// slot definitions: these should go in a header file somewhere
	#define DIGITAL_SLOT_1 	4
	
	#define MOTOR_L_PWM 		1
	#define MOTOR_R_PWM 		2
	
	#define ENCODER_L_SLOT	DIGITAL_SLOT_1
	#define ENCODER_L_IN1	1
	#define ENCODER_L_IN2	2
	
	#define ENCODER_R_SLOT	DIGITAL_SLOT_1
	#define ENCODER_R_IN1	3
	#define ENCODER_R_IN2	4
	
	#define WHEEL_RADIUS (0.1016 * (15.0 / 40.0))
	
	Encoder leftEncoder; // detects the speed that the wheels are turning at
	Encoder rightEncoder;
	
	WebDMA webdma;
	
	PIDMotor	leftMotor;
	PIDMotor	rightMotor;

	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	

public:
	RobotDemo(void):
		leftEncoder( ENCODER_L_SLOT, ENCODER_L_IN1, ENCODER_L_SLOT, ENCODER_L_IN2),
		rightEncoder( ENCODER_R_SLOT, ENCODER_R_IN1, ENCODER_R_SLOT, ENCODER_R_IN2),
		leftMotor( DIGITAL_SLOT_1, MOTOR_L_PWM, leftEncoder, PID_P, PID_I, PID_D, "L Motor", webdma ),
		rightMotor( DIGITAL_SLOT_1, MOTOR_R_PWM, rightEncoder, PID_P, PID_I, PID_D, "R Motor", webdma ),
		myRobot(leftMotor, rightMotor),
		stick(1)
	{
		GetWatchdog().SetExpiration(0.1);
		
		webdma.Enable();
		
		// enable the encoders
		leftEncoder.SetReverseDirection(true);
		
		leftEncoder.SetDistancePerPulse( (2 * M_PI * WHEEL_RADIUS) / 1440.0 );
		rightEncoder.SetDistancePerPulse( (2 * M_PI * WHEEL_RADIUS) / 1440.0 );
		
		leftEncoder.Start();
		rightEncoder.Stop();
		
		// enable the motors
		leftMotor.Enable();
		rightMotor.Enable();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			// lets just test with one wheel at first (y is inverted)
			leftMotor.Set( stick.GetY() * -1 );
			
			//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
		}
		
		leftMotor.Set(0);
	}
};

START_ROBOT_CLASS(RobotDemo);

