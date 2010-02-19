/*
 * This implements the vision mode.
 * 
 */


#include "WPILib.h"
#include "Vision.h"
#include <cmath>


#define VISION_TARGET_ACCURACY 2.5

#define VISION_P 0.1
#define VISION_I 0.0
#define VISION_D 0.5

#define VISION_PERIOD 0.005

#define VISION_TASK_PRIORITY 250


Vision::Vision(RobotResources& resources):
	m_resources(resources),
	m_turnController( 
				VISION_P, VISION_I, VISION_D, 
				this, // source
				this, // output
				"Vision", m_resources.webdma,
				VISION_PERIOD), // period
	m_camera(AxisCamera::GetInstance()),
	
	m_task( "Kwarqs Vision Task", (FUNCPTR)Vision::TimerFn, VISION_TASK_PRIORITY )
{	
	m_numTargets = m_resources.webdma.CreateIntProxy("Vision", "NumTargets",
		IntProxyFlags().readonly()
	);
	
	m_horizontalAngle = m_resources.webdma.CreateDoubleProxy("Vision", "Horizontal Angle",
		DoubleProxyFlags().readonly()
	);
	
	m_isRobotAligned = m_resources.webdma.CreateBoolProxy("Vision", "Aligned", false );
	m_setpointIsTarget = m_resources.webdma.CreateBoolProxy("Vision", "IsTarget", false );

	
	m_left_range = m_resources.webdma.CreateDoubleProxy("Vision", "Left Range",
		DoubleProxyFlags().readonly()
	);
	
	m_right_range = m_resources.webdma.CreateDoubleProxy("Vision", "Right Range",
		DoubleProxyFlags().readonly()
	);

	m_numTargets = 0;
	m_horizontalAngle = 0;
	m_isRobotAligned = false;
	
	/**
	 * Set up the PID controller with some parameters that should be pretty
	 * close for most kitbot robots.
	 */
	printf("Initializing PIDController\n");
	
	m_turnController.SetContinuous();
	m_turnController.SetInputRange(0.0, 360.0);
	m_turnController.SetOutputRange(-0.8, 0.8);
	m_turnController.SetTolerance(1.0 / 90.0 * 100);
	m_turnController.Disable();

	// Create and set up a camera instance. first wait for the camera to start
	// if the robot was just powered on. This gives the camera time to boot.
	printf("Setting m_camera parameters\n");
	m_camera.WriteResolution(AxisCamera::kResolution_320x240);
	m_camera.WriteCompression(20);
	m_camera.WriteBrightness(0);
	
	// create a mutex to allow us to synchronize access to variables
	// since we're going to be running in multiple threads
	m_mutex = semMCreate(0);

	// loop getting images from the camera and finding targets
	m_task.Start( (UINT32)this );
	
	printf("Vision Initialized\n");
}

Vision::~Vision()
{
	printf("Vision Destroyed\n");
}

void Vision::PreferLeft()
{
	Synchronized lock(m_mutex);
	
	m_left_range = 270.0;
	m_right_range = 360.0;
	
	double current_setpoint = m_turnController.GetSetpoint();
	
	if (current_setpoint < m_left_range || current_setpoint > m_right_range)
	{
		m_turnController.SetSetpoint(315.0);
		m_setpointIsTarget = false;
	}
	
	m_turnController.Enable();
}

void Vision::PreferEither()
{
	Synchronized lock(m_mutex);
	
	m_left_range = 270.0;
	m_right_range = 90.0;
	
	
	
	m_turnController.Enable();
}

void Vision::PreferRight()
{
	Synchronized lock(m_mutex);
	
	m_left_range = 0.0;
	m_right_range = 90.0;
	
	double current_setpoint = m_turnController.GetSetpoint();
	
	if (current_setpoint < m_left_range || current_setpoint > m_right_range)
	{
		m_turnController.SetSetpoint(45.0);
		m_setpointIsTarget = false;
	}
	
	m_turnController.Enable();
}

void Vision::DisableMotorControl()
{
	m_turnController.Disable();
}


int Vision::TimerFn(void * param)
{
	((Vision*)param)->ProcessVision();
	return 0;
}

void Vision::ProcessVision()
{
	SEM_ID camera_ready = m_camera.GetNewImageSem();
	
	
	// if there's a fresh and we're at the previous target heading then
	// get a camera image and process it
	while (true)
	{
		// wait for the semaphore
		semTake(camera_ready, WAIT_FOREVER);
		
		double horizontalAngle = 0;
	
		// get the m_resources.gyro heading that goes with this image
		double gyroAngle = PIDGet();

		// get the camera image
		HSLImage *image = m_camera.GetImage();

		// find FRC targets in the image
		vector<Target> targets ( Target::FindCircularTargets(image) );
		delete image;
		
		if (targets.size() == 0 || targets[0].m_score < MINIMUM_SCORE)
		{
			// no targets found. Make sure the first one in the list is 0,0
			// since the dashboard program annotates the first target in green
			// and the others in magenta. With no qualified targets, they'll all
			// be magenta.
			Target nullTarget;
			nullTarget.m_majorRadius = 0.0;
			nullTarget.m_minorRadius = 0.0;
			nullTarget.m_score = 0.0;
			if (targets.size() == 0)
				targets.push_back(nullTarget);
			else
				targets.insert(targets.begin(), nullTarget);
			
			m_dds.sendVisionData(0.0, m_resources.gyro.GetAngle(), 0.0, 0.0, targets);
		}
		else {
			// We have some targets.
			// set the new PID heading setpoint to the first target in the list
			horizontalAngle = targets[0].GetHorizontalAngle();
			double setPoint = gyroAngle + horizontalAngle;

			// only set the setpoint if we're pointing the right direction
			{
				Synchronized lock(m_mutex);
				
				if ((m_right_range > m_left_range && 
						setPoint > m_left_range && setPoint < m_right_range ) ||
					(m_right_range < m_left_range &&
						(setPoint > m_left_range || setPoint < m_right_range)))
				{
					m_turnController.SetSetpoint(setPoint);
					m_setpointIsTarget = true;
				}
			}
			
			// send dashboard data for target tracking
			m_dds.sendVisionData(0.0, m_resources.gyro.GetAngle(), 0.0, targets[0].m_xPos / targets[0].m_xMax, targets);
			//printf("Target found %f ", targets[0].m_score);
			// targets[0].Print();
		}
		
		
		// WebDMA output stuff:
		m_horizontalAngle = horizontalAngle;
		m_numTargets = targets.size();
		
		m_isRobotAligned = m_turnController.OnTarget();
		
		// autosweep!
		if ( m_isRobotAligned && !m_setpointIsTarget)
		{
			Synchronized lock(m_mutex);
			
			double current_setpoint = m_turnController.GetSetpoint();
			
			double left_distance = m_left_range - current_setpoint;
			if (left_distance < 0 )
				left_distance = 360 - left_distance;
					
			double right_distance = m_right_range - current_setpoint;
			if (right_distance < 0)
				right_distance = 360.0 - right_distance;
			
			if (right_distance > left_distance)
			{
				double new_point = m_right_range - 22.5;
				if (new_point < 0)
					new_point = 360 - new_point;
				
				m_turnController.SetSetpoint( new_point );
			}
			else
			{
				double new_point = m_left_range + 22.5;
				if (new_point < 0)
					new_point = 360 - new_point;
				
				m_turnController.SetSetpoint( new_point );
			}
		}
	
		
		// send the dashboard data associated with the I/O ports
		m_dds.sendIOPortData();
	}
}

bool Vision::IsRobotPointingAtTarget() const 
{
	// doesn't need a mutex since WebDMA already locks for us
	return m_isRobotAligned && m_setpointIsTarget;
}

void Vision::PIDWrite(float output)
{
	m_resources.myRobot.ArcadeDrive(0.0, output);
}

double Vision::PIDGet()
{
	double angle = fmod(m_resources.gyro.PIDGet(), 360.0);
	
	if (angle < 0)
		return 360.0 + angle;
	
	return angle; 	
}
