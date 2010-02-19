/*
 * This implements the vision mode.
 * 
 */


#include "WPILib.h"
#include "Vision.h"
#include "kwarqs_math.h"


#define VISION_MINIMUM_SCORE 0.01

#define VISION_P 0.1
#define VISION_I 0.0
#define VISION_D 0.5

#define VISION_PERIOD 0.005

#define VISION_TASK_PRIORITY 250

#define VISION_AUTOSWEEP_EDGE 22.5

// PreferLeft parameters
#define VISION_PL_LEFT 		270.0
#define VISION_PL_RIGHT		360.0
#define VISION_PL_SETPOINT	315.0

#define VISION_PE_LEFT		270.0
#define VISION_PE_RIGHT		90.0

// PreferRight parameters
#define VISION_PR_LEFT		0.0
#define VISION_PR_RIGHT		90.0
#define VISION_PR_SETPOINT	45.0


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

	
	m_left_edge = m_resources.webdma.CreateDoubleProxy("Vision", "Left Edge",
		DoubleProxyFlags().readonly()
	);
	
	m_right_edge = m_resources.webdma.CreateDoubleProxy("Vision", "Right Edge",
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
	
	m_left_edge = VISION_PL_LEFT;
	m_right_edge = VISION_PL_RIGHT;
	
	if (!AngleWithinEdges( m_turnController.GetSetpoint()))
	{
		m_turnController.SetSetpoint(VISION_PL_SETPOINT);
		m_setpointIsTarget = false;
	}
	
	m_turnController.Enable();
}

void Vision::PreferEither()
{
	Synchronized lock(m_mutex);
	
	m_left_edge = VISION_PE_LEFT;
	m_right_edge = VISION_PE_RIGHT;
	
	m_turnController.Enable();
}

void Vision::PreferRight()
{
	Synchronized lock(m_mutex);
	
	m_left_edge = VISION_PR_LEFT;
	m_right_edge = VISION_PR_RIGHT;
	
	if (!AngleWithinEdges( m_turnController.GetSetpoint()))
	{
		m_turnController.SetSetpoint(VISION_PR_SETPOINT);
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
	
	// if there's a fresh image then get a camera image and process it
	while (true)
	{
		// wait for the semaphore
		semTake(camera_ready, WAIT_FOREVER);
		
		double horizontalAngle = 0;
	
		// get the gyro heading that goes with this image
		double gyroAngle = PIDGet();

		// get the camera image
		HSLImage *image = m_camera.GetImage();

		// find FRC targets in the image
		vector<Target> targets ( Target::FindCircularTargets(image) );
		delete image;
		
		if (targets.size() == 0 || targets[0].m_score < VISION_MINIMUM_SCORE)
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
			
			m_dds.sendVisionData(0.0, gyroAngle, 0.0, 0.0, targets);
		}
		else 
		{
			// We have some targets.
			// set the new PID heading setpoint to the first target in the list
			horizontalAngle = targets[0].GetHorizontalAngle();
			double setPoint = angle_normalize( gyroAngle + horizontalAngle );
			
			bool send_tgt_vision = false;

			// only set the setpoint if the target resides in the
			// range we currently allow targets to reside in
			{
				Synchronized lock(m_mutex);
				
				if (AngleWithinEdges( setPoint ))
				{
					m_turnController.SetSetpoint(setPoint);
					m_setpointIsTarget = true;
					
					send_tgt_vision = true;
				}
			}
			
			// send dashboard data for target tracking
			// note: we only set the target IF we selected it (ie, only if it fell in our selected range)
			if (send_tgt_vision)
				m_dds.sendVisionData(0.0, gyroAngle, 0.0, targets[0].m_xPos / targets[0].m_xMax, targets);
			else
				m_dds.sendVisionData(0.0, gyroAngle, 0.0, 0.0, targets);

		}
		
		// WebDMA output stuff:
		m_horizontalAngle = horizontalAngle;
		m_numTargets = targets.size();
		
		m_isRobotAligned = m_turnController.OnTarget();
		
		// only sweep if there isn't a target and we're at the current
		// setpoint for the robot
		if ( m_isRobotAligned && !m_setpointIsTarget)
			AutoSweep();
		
		// send the dashboard data associated with the I/O ports
		m_dds.sendIOPortData();
	}
}


// code to do autosweeping
void Vision::AutoSweep()
{
	double left_edge, right_edge;

	{
		// grab these variables in this block so we don't hold the lock for as long
		Synchronized lock(m_mutex);
		left_edge = m_left_edge;
		right_edge = m_right_edge;
	}
	
	double current_setpoint = m_turnController.GetSetpoint();
	
	// get distances
	double left_distance = angle_distance(current_setpoint, left_edge);
	double right_distance = angle_distance(current_setpoint, right_edge);

	if (right_distance > left_distance)
		m_turnController.SetSetpoint( angle_normalize( right_edge - VISION_AUTOSWEEP_EDGE ) );
	else
		m_turnController.SetSetpoint( angle_normalize( left_edge + VISION_AUTOSWEEP_EDGE) );
}


bool Vision::AngleWithinEdges(double angle) const
{
	// read these first since WebDMA does locks
	double right_edge = m_right_edge;
	double left_edge = m_left_edge;
	
	// dont screw with this statement, it works and has been tested
	return ((right_edge > left_edge && 
			angle > left_edge && angle < right_edge ) ||
		(right_edge < left_edge &&
			(angle > left_edge || angle < right_edge)));
}

bool Vision::IsRobotPointingAtTarget() const 
{
	// doesn't need a mutex since WebDMA already locks for us
	
	// note that this checks for both the robot being
	// aligned and for the setpoint to be on target. One
	// or the other isn't sufficient, you need both to be
	// certain
	return m_isRobotAligned && m_setpointIsTarget;
}

void Vision::PIDWrite(float output)
{
	// first parameter is zero because we want to spin in place
	m_resources.myRobot.ArcadeDrive(0.0, output);
}

double Vision::PIDGet()
{
	// this returns a normalized angle from the gyro
	return angle_normalize( m_resources.gyro.PIDGet() );	
}
