/**
	\file 		Vision.cpp
	\author 	Dustin Spicuzza, Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/



#include "WPILib.h"
#include "Vision.h"
#include "kwarqs_math.h"


#define VISION_MINIMUM_SCORE 0.01


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
	m_camera(AxisCamera::GetInstance()),
	
	m_task( "Kwarqs Vision Task", (FUNCPTR)Vision::TimerFn, VISION_TASK_PRIORITY )
{	
	m_numTargets = resources.webdma.CreateIntProxy("Vision", "NumTargets",
		IntProxyFlags().readonly().default(0)
	);
	
	m_horizontalAngle = resources.webdma.CreateDoubleProxy("Vision", "Horizontal Angle",
		DoubleProxyFlags().readonly().default(0)
	);
	
	m_isRobotAligned = resources.webdma.CreateBoolProxy("Vision", "Aligned", false );
	m_setpointIsTarget = resources.webdma.CreateBoolProxy("Vision", "IsTarget", false );

	
	m_left_edge = resources.webdma.CreateDoubleProxy("Vision", "Left Edge",
		DoubleProxyFlags().readonly().default(VISION_PE_LEFT)
	);
	
	m_right_edge = resources.webdma.CreateDoubleProxy("Vision", "Right Edge",
		DoubleProxyFlags().readonly().default(VISION_PE_RIGHT)
	);
	
	m_setpoint = resources.webdma.CreateDoubleProxy("Vision", "Setpoint",
		DoubleProxyFlags().readonly().default(0)
	);
	
	m_enabled = resources.webdma.CreateBoolProxy("Vision", "Enabled", false );

	m_numTargets = 0;
	m_horizontalAngle = 0;
	m_isRobotAligned = false;

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
	semDelete(m_mutex);
}

void Vision::PreferLeft()
{
	Synchronized lock(m_mutex);
	
	m_left_edge = VISION_PL_LEFT;
	m_right_edge = VISION_PL_RIGHT;
	
	if (!AngleWithinEdges( m_setpoint ))
		m_setpoint = VISION_PL_SETPOINT;
	
	m_setpointIsTarget = false;
}

void Vision::PreferEither()
{
	Synchronized lock(m_mutex);
	
	m_left_edge = VISION_PE_LEFT;
	m_right_edge = VISION_PE_RIGHT;
	
	m_setpointIsTarget = false;
}

void Vision::PreferRight()
{
	Synchronized lock(m_mutex);
	
	m_left_edge = VISION_PR_LEFT;
	m_right_edge = VISION_PR_RIGHT;
	
	if (!AngleWithinEdges( m_setpoint ))
		m_setpoint = VISION_PR_SETPOINT;
	
	m_setpointIsTarget = false;
}

double Vision::GetVisionAngle()
{
	Synchronized lock(m_mutex);
	return m_setpoint;
}

bool Vision::IsRobotPointingAtTarget() const 
{
	Synchronized lock(m_mutex);
	
	// note that this checks for both the robot being
	// aligned and for the setpoint to be on a target. One
	// or the other isn't sufficient, you need both to be
	// certain
	return m_isRobotAligned && m_setpointIsTarget;
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
		double gyroAngle = m_turnController.PIDGet();
		double targetPos = 0.0;

		// get the camera image
		HSLImage *image = m_camera.GetImage();

		// find FRC targets in the image
		vector<Target> targets ( Target::FindCircularTargets(image) );
		delete image;
		
		{
			// this block needs to be synchronized
			Synchronized lock(m_mutex);
		
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
			}
			else 
			{
				// We have some targets.
				// set the new PID heading setpoint to the first target in the list
				horizontalAngle = targets[0].GetHorizontalAngle();
				double setPoint = angle_normalize( gyroAngle + horizontalAngle );

				// only set the setpoint if the target resides in the
				// range we currently allow targets to reside in

				if (AngleWithinEdges( setPoint ))
				{
					m_setpoint = setPoint;
					m_setpointIsTarget = true;
				
					targetPos = targets[0].m_xPos / targets[0].m_xMax;
				}
			}
		
			// WebDMA output stuff:
			m_horizontalAngle = horizontalAngle;
			m_numTargets = targets.size();
		
			m_isRobotAligned = m_turnController.OnTarget();
		
			// only sweep if there isn't a target and we're at the current
			// setpoint for the robot
			if ( m_isRobotAligned && !m_setpointIsTarget)
				AutoSweep();
		}
			
		m_dds.sendVisionData(0.0, gyroAngle, 0.0, targetPos, targets);
		
		// send the dashboard data associated with the I/O ports
		m_dds.sendIOPortData();
	}
}


// code to do autosweeping
// note: must be protected by mutex
void Vision::AutoSweep()
{
	// read these first since WebDMA does locks
	double left_edge = m_left_edge;
	double right_edge = m_right_edge;
	
	// get distances
	double left_distance = angle_distance(m_setpoint, left_edge);
	double right_distance = angle_distance(m_setpoint, right_edge);

	if (right_distance > left_distance)
		m_setpoint = angle_normalize( right_edge - VISION_AUTOSWEEP_EDGE );
	else
		m_setpoint = angle_normalize( left_edge + VISION_AUTOSWEEP_EDGE);
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


