/********************************************************************************
*  Project   		: FIRST Motor Controller
*  File Name  		: VisionDemo.cpp        
*  Contributors 	: ELF
*  Creation Date 	: Aug 27, 2008
*  Revision History	: Source code & revision history maintained at sourceforge.WPI.edu   
*  File Description	: Demo program showing color tracking
*/                         
/*----------------------------------------------------------------------------*/
/*        Copyright (c) FIRST 2008.  All Rights Reserved.                     */
/*  Open Source Software - may be modified and shared by FRC teams. The code  */
/*  must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib. */
/*----------------------------------------------------------------------------*/

#include <iostream.h>
#include "math.h"

#include "AxisCamera.h" 
#include "BaeUtilities.h"
#include "FrcError.h" 
#include "PCVideoServer.h" 
#include "TrackAPI.h"
#include "WPILib.h"

// To locally enable debug printing: set VisionDemo_debugFlag to a 1, to disable set to 0
int VisionDemo_debugFlag = 0;
#define DPRINTF if(VisionDemo_debugFlag)dprintf

#define MIN_PARTICLE_TO_IMAGE_PERCENT 0.38		// target is too small
#define MAX_PARTICLE_TO_IMAGE_PERCENT 10.0		// target is too close

/**
 * This is a demo program showing the use of the color tracking API. 
 * It uses the SimpleRobot class as a base of a robot application that will automatically call 
 * your Autonomous and OperatorControl methods at the right time as controlled by the switches
 * on the driver station or the field controls. Autonomous mode tracks color assumes a camera
 * on a fixed mount. Options for autonomous include driving forward or backward if a color is 
 * detected, following a color, stoplight mode (forward if green, backwards if red), and
 * taking a snapshot (not related to color tracking but useful at times).
 */
class VisionDemo : public SimpleRobot
{
	RobotDrive *myRobot;			// robot drive system
	Joystick *rightStick;			// joystick 1 (arcade stick or right tank stick)
	Joystick *leftStick;			// joystick 2 (tank left stick)
	DriverStation *ds;				// driver station object
	int framesPerSecond;			// image acquisition
	
	// data structures for calls to FindColor		
	ParticleAnalysisReport par, par2;	// particle analysis reports
	ColorReport cReport;			// color report
	TrackingThreshold redData; 		// image data for tracking
	TrackingThreshold greenData; 	// image data for tracking
	TrackingThreshold purpleData; 	// image data for tracking
	
	enum visionAction {GO_FORWARD, FOLLOW_COLOR, MOVE_AWAY, STOPLIGHT, SNAPSHOT};
	
	enum							// Driver Station jumpers to control program operation
	{ ARCADE_MODE = 1,				// Tank/Arcade jumper is on DS Input 1 (Jumper present is arcade)
	  ENABLE_AUTONOMOUS = 2,		// Autonomous/Teleop jumper is on DS Input 2 (Jumper present is autonomous)
	} jumpers;	                            

public:
	/**
	 * Constructor for this robot subclass.
	 * Create an instance of a RobotDrive with left and right motors plugged into PWM
	 * ports 1 and 2 on the first digital module.
	 */
	VisionDemo(void)
	{
		char funcName[]="VisionDemo";
		ds = DriverStation::GetInstance();
		myRobot = new RobotDrive(1, 2, 0.5);	// robot will use PWM 1-2 for drive motors
		rightStick = new Joystick(1);			// create the joysticks
		leftStick = new Joystick(2);

#if 1
		
		
		/* set up debug output: 
		 * DEBUG_OFF, DEBUG_MOSTLY_OFF, DEBUG_SCREEN_ONLY, DEBUG_FILE_ONLY, DEBUG_SCREEN_AND_FILE 
		 */
		SetDebugFlag ( DEBUG_SCREEN_ONLY  ) ;
		
		/* start the CameraTask	 */
		framesPerSecond = 10;
		if (StartCameraTask(framesPerSecond, 0, k160x120, ROT_0) == -1) 
		{
			printf( LOG_ERROR,"Failed to spawn camera task; Error code %i", 
					GetVisionErrorText(GetLastVisionError()) );
		}
		
		/* allow writing to vxWorks target */
		Priv_SetWriteFileAllowed(1);   		
#endif
		/* stop the watchdog if debugging  */
		GetWatchdog().SetEnabled(false);
	}
	
	/** Simple test to see if the color is taking up more than 10$ of the image */
	int tooClose(ParticleAnalysisReport* par)	{
		if (par->particleToImagePercent > MAX_PARTICLE_TO_IMAGE_PERCENT) return 1;
		return 0;
	}			
	/** Simple test to see if the color is taking up too little of the image */
	int tooSmall(ParticleAnalysisReport* par)	{
		if (par->particleToImagePercent < MIN_PARTICLE_TO_IMAGE_PERCENT) {
			return 1;
		}
		return 0;
	}	

	void Autonomous(void)
	{
		char funcName[]="Autonomous";
		DPRINTF(LOG_DEBUG, "start VisionDemo autonomous");			
		//GetWatchdog().Feed(); 
		visionAction va = STOPLIGHT; 
		
		float mySpeed = 1.0;			// this may be fast - watch where you stand!
		float driveDirection = 0.0;		// tasking
		float driveSpeed = 0.0;			// tasking
		int greenCount = 0;
		int redCount = 0;
		int minColorFilterCount = 3;	
		double redsize, greensize;		
		
		// color values 
		redData = GetTrackingData(RED, FLUORESCENT); 
		greenData = GetTrackingData(GREEN, FLUORESCENT); 
		purpleData = GetTrackingData(PURPLE, FLUORESCENT); 
		
		// parameters for taking a snapshot
		Image* cameraImage;
		// Put image a root directory. If another directory is used
		// it must exist on the cRIO.
		char* imageName = "/demo.png"; 
				
		int cont = 1;
		while( 	(IsAutonomous()) && cont )	
		{		
		 
			switch (va) {
			
				case GO_FORWARD:
					/* this simple test will drive forward if COLOR is detected
					 * drive will last until autonomous terminates
					 */
					DPRINTF(LOG_DEBUG, "GO_FORWARD - looking for GREEN ");
					// simple call to FindColor
					driveSpeed = 0.0;
					driveDirection = 0.0;
					if ( FindColor(GREEN, &par) 
								&& par.particleToImagePercent < MAX_PARTICLE_TO_IMAGE_PERCENT
								&& par.particleToImagePercent > MIN_PARTICLE_TO_IMAGE_PERCENT) {
						//PrintReport(par);  // shows particle statistics
						if (tooClose(&par)) {
							ShowActivity ("GREEN found - TOO CLOSE!    ");
						} else if ( tooSmall(&par) ) {
							DPRINTF(LOG_DEBUG, "GREEN particle too small");
							ShowActivity ("dead stop: GREEN particle too small      ");		
						} else {
							// drive forwards 
							driveSpeed = mySpeed;
							ShowActivity ("GREEN found - going forward ");
						}
					} else {
						DPRINTF(LOG_DEBUG, "no GREEN found");
						ShowActivity ("dead stop: no GREEN found   ");
					}
					myRobot->Drive(driveSpeed, driveDirection);
					break;
					
				case FOLLOW_COLOR:
					/* this simple test will drive toward RED 
					 * drive will last until autonomous terminates
					 */
					DPRINTF(LOG_DEBUG, "FOLLOW_COLOR - looking for GREEN ");	
					driveSpeed = 0.0;
					driveDirection = 0.0;
					// complex call to FindColor		
					if ( FindColor(IMAQ_HSL, &greenData.hue, &greenData.saturation, 
										&greenData.luminance, &par, &cReport) ) {
						//PrintReport(&cReport);	// shows particle color data					
						if ( tooClose(&par) ) {
							ShowActivity ("GREEN found - TOO CLOSE!      ");
						} else if ( tooSmall(&par) ) {
							ShowActivity ("GREEN found - TOO SMALL!      ");
						}  else {
							// drive toward color
							driveDirection = (float)(par.center_mass_x_normalized * -1.0);
							driveSpeed = mySpeed;
							ShowActivity ("GREEN found - following x value: %f          ",
									driveDirection);	
						} 
					} else {
						DPRINTF(LOG_DEBUG, "no GREEN found");
						ShowActivity ("dead stop: no GREEN found                        ");
					}
					myRobot->Drive(driveSpeed, driveDirection);
					break;
					
				case MOVE_AWAY:
					/* this simple test move away from purple
					 * drive will last until autonomous terminates
					 */
					DPRINTF(LOG_DEBUG, "MOVE_AWAY - looking for PURPLE ");
					
					driveSpeed = 0.0;
					driveDirection = 0.0;
					
					if (FindColor(PURPLE, &par)) {
						// move away from color
						ShowActivity ("PURPLE found - going backwards ");
						driveSpeed = -mySpeed;		// reverse 		
					} else {
						DPRINTF(LOG_DEBUG, "no PURPLE found");
						ShowActivity ("dead stop: no PURPLE found     ");
					}
					myRobot->Drive(driveSpeed, driveDirection);
					break;
					
				case STOPLIGHT:
					/* this simple test move forward for green
					 * and move away for red 
					 */
					//DPRINTF(LOG_DEBUG, "STOPLIGHT - looking for RED/GREEN ");
					redsize = 0; greensize = 0.0;
					
					/* find the size of the largest particle of each color */
					if ( FindColor(IMAQ_HSL, &greenData.hue, &greenData.saturation, 
										&greenData.luminance, &par) ) {
						greensize = par.particleArea;
					}
					if ( FindColor(IMAQ_HSL, &redData.hue, &redData.saturation, 
										&redData.luminance, &par2) ) {
						redsize = par2.particleArea;
					}
					
					/* move based on the larger of the two colors seen 
					 * if green, go forwards, if red, go backwards
					 */
					driveSpeed = 0.0;
					driveDirection = 0.0;
					
					if (greensize > redsize)
					{
						greenCount++; redCount=0;
						if ( (greenCount >= minColorFilterCount) && (! tooClose(&par) )) {
								DPRINTF(LOG_DEBUG, "GREEN found  ");
								ShowActivity ("GREEN found - going forward ");
								driveSpeed = mySpeed;
						} else {
								ShowActivity ("GREEN found - TOO CLOSE!      ");
						}				
					} else if (redsize > 0.0) {
						greenCount=0; redCount++;
						if ( redCount >= minColorFilterCount ) {
					    	// going backwards - no need to check closeness
							DPRINTF(LOG_DEBUG, "RED found   ");
							ShowActivity ("RED found - going backwards ");
							driveSpeed = -mySpeed;		// reverse 		
						}
					} else {
						//neither color seen - stop immediately
						DPRINTF(LOG_DEBUG, "dead stop ");
						ShowActivity ("dead stop                     ");
					}
					myRobot->Drive(driveSpeed, driveDirection);
					break;

				case SNAPSHOT:
					/* this will take one picture and save it to a file
					 */
					DPRINTF(LOG_DEBUG, "taking a SNAPSHOT ");
					cameraImage = frcCreateImage(IMAQ_IMAGE_HSL);
					if (!cameraImage) {
						dprintf (LOG_INFO,"frcCreateImage failed with errorcode %i",GetLastVisionError()); 
						break;
					}
					Wait(1.0);
					if ( !GetImage (cameraImage,NULL) ) {
						  dprintf (LOG_INFO,"\nCamera Acquisition failed %i", GetLastVisionError());
					} else { 
						  if (!frcWriteImage(cameraImage, imageName) ) { 
								dprintf (LOG_INFO,"frcWriteImage failed with errorcode %i",GetLastVisionError());
						  } else { 
							  	dprintf (LOG_INFO,"\nSaved image to %s", imageName);	
								// always dispose of image objects when done
								frcDispose(cameraImage);
								cont = 0;
								break;
						  }
					}
					break;
					
				default:
					break;
			}  // end switch

			Wait(0.05);		
		}  // end while
		
		myRobot->Drive(0.0, 0.0);		// stop robot
		DPRINTF(LOG_DEBUG, "end VisionDemo autonomous");
		ShowActivity ("VisionDemo autonomous end                                            ");
		
	}  // end autonomous

	/**
	 * Runs the motors under driver control with either tank or arcade steering selected
	 * by a jumper in DS Digin 0. 
	 */
	void OperatorControl(void)
	{
		char funcName[]="OperatorControl";
		DPRINTF(LOG_DEBUG, "OperatorControl");
		char* imageName = "rob.png"; 

 
		double start = GetTime();
		Image* cameraImage = frcCreateImage(IMAQ_IMAGE_HSL);
		
		if (!cameraImage) {
			printf (LOG_INFO,"frcCreateImage failed with errorcode %i",GetLastVisionError()); 
			return;
		}
        
		//RJM - don't fully know why this delay is needed here ... BUT IT IS ???
		while( GetTime()- start < 5);
		
		double timestamp; // timestamp of image returned
		double lastImageTimestamp; // timestamp of last image, to ensure image is new

		int success = GetImageBlocking(cameraImage, &timestamp, lastImageTimestamp);		
		printf("IMAGE GET was %d\n", success);
				
		if (!frcWriteImage(cameraImage, imageName) ) 
		{ 
		    //printf (LOG_INFO,"frcWriteImage failed with errorcode %i",GetLastVisionError());
				  
		} 
		
        start = GetTime();
		while(1)
		{
			if( GetTime() - start > 1)
			{
				printf("Hello\n");
				start = GetTime();
			}
		}
			

				
				
			
	} // end operator control
	
}; 

// entry point is FRC_UserProgram_StartupLibraryInit
START_ROBOT_CLASS(VisionDemo);

