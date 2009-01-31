#include "WPILib.h"
#include <cmath>
#include <VisionAPI.h>

#include "UDPClient.h"
#include "positionTracking.h"

#define MY_PI 3.14



/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	Jaguar leftMotor;
	Jaguar rightMotor;
	
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	
	Gyro gyro;
	
	Accelerometer accelerometerX;
	Accelerometer accelerometerY;
	
	Encoder encoderRight;
	Encoder encoderLeft;
	
	// moving right is positive

public:
	RobotDemo(void):
		leftMotor(1),
		rightMotor(2),
		myRobot(&leftMotor, &rightMotor),
		stick(1),
		gyro(1),
		accelerometerX(3),
		accelerometerY(4),
		encoderRight(3,4, false),
		encoderLeft(5,6,false)
	{
		GetWatchdog().SetEnabled(false);
		gyro.SetSensitivity(0.007);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		/*
		GetWatchdog().SetEnabled(false);
		myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
		*/
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		printf("Entered OperatorControl\n");
		
		double time = GetTime(), start = GetTime(), printTime = GetTime();
		float accX, accY;
		float velX, velY, posX, posY;
        int encoderL, encoderR;
        float gyroAngle;
		
		
		GetWatchdog().SetEnabled(true);
		
		
		// allow writing to file system
		Priv_SetWriteFileAllowed(1);
		
		char fname[128];
		sprintf(fname, "sensors-%d.out", (int)time );
		
		FILE * fSensor = fopen(fname, "wb");
		if (!fSensor)
		{
			fprintf(stderr, "%s could not be opened\n", fname);
		}
		//fprintf(fSensor,"AccelX VelX PosX AccelY PosY Gyro EncoderLR Encoder RR\n");
		
		
		
		// reset the accelerometer object and set the acceleromter biases
	    AccelerationReset();
	    
	    //start the encoders
	    encoderLeft.Start();
	    encoderRight.Start();
		
		// cal the accelerometer biases
		time = GetTime();
	    while( GetTime() - start < 5 )
	    {
		  if( GetTime() - time > .1 )
		  {
			accX = accelerometerX.GetAcceleration() * 9.81;
			accY = accelerometerY.GetAcceleration() * 9.81;
	    	AccelerationUpdate( accX, accY, .1);
            time = GetTime();
		  }
	    }
	    start = GetTime();
	    time = GetTime();
	    
		
		float accBiasX, accBiasY;
		AccelerationEstimateBias( &accBiasX, &accBiasY);
		AccelerationSetBias( accBiasX, accBiasY);
		printf("Done with cal accX:%1.4f accY:%1.4f ...\n",accBiasX,accBiasY);
		
					
	    while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			

			//write the first 30 seconds to file
			if (GetTime() - start > 30)
			{
				if (fSensor)
				{
					printf("Done writing %s.\n", fname);
					fclose(fSensor);
					fSensor = NULL;
				}
				
			}
			
			//update the sensors every 100ms
			if (GetTime() - time > 0.1)
			{
				
				//read the sensor data
				gyroAngle = gyro.GetAngle();
				
				encoderL = encoderLeft.Get();
				encoderR =  encoderRight.Get();
				
				
				
				// convert g's to units of meters/sec	
				accX = accelerometerX.GetAcceleration() * 9.81;
				accY = accelerometerY.GetAcceleration() * 9.81;

				//update the tracking object
				AccelerationUpdate( accX, accY, .1);
				
				//get the filtered acceleration, velocity and position
				GetAcceleration( &accX, &accY);
				GetVelocity( &velX, &velY );
				GetPosition( &posX, &posY );
								
				//log the sensor values
				if( fSensor)
				{
				  fwrite((char *)&accX, sizeof(float),1, fSensor);
				  fwrite((char *)&velX, sizeof(float),1, fSensor);
				  fwrite((char *)&posX, sizeof(float),1, fSensor);
				  fwrite((char *)&accY, sizeof(float),1, fSensor);
				  fwrite((char *)&velY, sizeof(float),1, fSensor);
				  fwrite((char *)&posY, sizeof(float),1, fSensor);				
				  fwrite((char *)&gyroAngle, sizeof(float),1, fSensor);
				  fwrite((char *)&encoderL, sizeof(int), 1, fSensor);
				  fwrite((char *)&encoderR, sizeof(int), 1, fSensor);
				}

				#if 0
				printf("%f %f %f %f %f %f %f %d %d\r",
						asccX, velX, posX, 
						accY, velY, posY,
						gyroAngle, 
						encoderLeft.Get(), encoderRight.Get());
                #endif
				
				time = GetTime();
			}
			
			if( GetTime() - printTime > 1)
			{
				printf("EncL:%d EncR:%d\n",encoderL,encoderR);
				printTime = GetTime();
			}
			
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

