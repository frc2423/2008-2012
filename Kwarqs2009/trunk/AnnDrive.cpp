/**
	\file 		SwerveDrive.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

/*
 * Copyright (c) 2009 Dustin Spicuzza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Dustin Spicuzza nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DUSTIN SPICUZZA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DUSTIN SPICUZZA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <WPILib.h>
#include "SwerveDrive.h"

#include "Framework/KwarqsConstants.h"
#include "Framework/math.h"
#include "Framework/DriverStationLCD.h"

// default constructor
SwerveDrive::SwerveDrive(RobotChassis * chassis) :
	m_chassis(chassis),
	m_time(GetTime())
{}






void SwerveDrive::Move(
	double &speed, 
	double &angle, 
	double &rotation,
	bool &stop
)
{
	/*
	if (GetTime() - m_time > 0.1)
	{		
		DriverStationLCD * lcd = DriverStationLCD::GetInstance();
		
		//lcd->Printf(DriverStationLCD::kUser_Line2, 1, "S: %.1f A: %.1f R: %.1f          ",
		//		speed, angle, rotation);
		
		lcd->Printf(DriverStationLCD::kUser_Line3, 1, "LF: %.1f %.1f %.1f          ",
			m_chassis->servo_lf.GetSetAngle(),
			m_chassis->servo_lf.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);
		
		lcd->Printf(DriverStationLCD::kUser_Line4, 1, "LR: %.1f %.1f %.1f          ",
			m_chassis->servo_lr.GetSetAngle(),
			m_chassis->servo_lr.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);
		
		lcd->Printf(DriverStationLCD::kUser_Line5, 1, "RF: %.1f %.1f %.1f          ",
			m_chassis->servo_rf.GetSetAngle(),
			m_chassis->servo_rf.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);

		lcd->Printf(DriverStationLCD::kUser_Line6, 1, "RR: %.1f %.1f %.1f          ",
			m_chassis->servo_rr.GetSetAngle(),
			m_chassis->servo_rr.GetCurrentAngle(),
			m_chassis->motor_lr.GetSetSpeed()
		);
		
		lcd->UpdateLCD();
		m_time = GetTime();
	}
	*/
	
	
	if (stop)
	{
		Stop();
		return;
	}
	
	/*
		Thoughts and notes
		
		We should calculate an ideal speed and angle for each wheel (ie, 
		our goal), and tell the wheel servos to go there. However, we must
		set the velocities of the wheels based on the current angle of the 
		wheels so that we don't go in weird directions
		
		Question: is this actually true? Should we just sit until the wheels
		turn (this seems like a bad idea)
		
		Currently, neither of those is implemented.
		
	*/
	
	// special case: doing nothing
	if (fabs(speed) < 0.0001 && fabs(angle) < 0.0001 && fabs(rotation) < 0.0001)
	{
		// this forces motors to stop and keeps the wheels pointed in the
		// same direction, much less annoying
		m_chassis->motor_lf.SetSpeed(0);
		m_chassis->motor_lr.SetSpeed(0);
		m_chassis->motor_rf.SetSpeed(0);
		m_chassis->motor_rr.SetSpeed(0);
		
		return;
	}
	
	
	// set limitations on speed
	if (speed < -1.0)
		speed = -1.0;
	else if (speed > 1.0)
		speed = 1.0;
	
	// set limitations on rotation
	if (rotation < -1.0)
		rotation = -1.0;
	else if (rotation > 1.0)
		rotation = 1.0;
 
	
//----------------------------------------------------------------------------------------------
// ANNETTE--added lots of stuff
//----------------------------------------------------------------------------------------------


// I'm assuming this method's parameter, angle (for the basic direction the wwheels are facing), is in radians,
// and that 0 means pointing so that the robot would be driving straight front.

// For this, I'm assuming that twisting the joystick will mean have robot drive in a curve, rotating along the way.

// The "key point" should be roughly the center of mass of the robot-and-trailer combination.
// Since it should be on the mid-line, only its distance back from the front wheels is needed.

// It might be worthwhile experimenting to find the best value to use for that, for the "big_r"
// that affects furthest center of rotation (shallowest curve), for the fudge factor that affects
// the closest center of rotation (shallowest curve, and for the function. 
 


// constants

const double length=18.5, width=29.5, key_d=12;	// robot shape, and key point distance from front

const double big_r=100; 			// longest possible distance between key and center of rotation
						// for shallowest curve (that's not a straight line)
						// maybe make it close to the field size (54')
const double PI = 3.1415926;
const double half_pi = PI/2;
const double to_radians = 180./PI;
const double to_degrees = PI/180.;
const double almost_0 = .01;
const double fudge = 1.0;			// fudge factor for computing small_r
				
const double half_w=width/2;	// half width
const double lf_x=key_d, lr_x=key_d-length,
			rf_x=key_d, rr_x=key_d-length; 	//x coordinates of wheels
const double lf_y=half_w, lr_y=half_w, rf_y=-half_w,
			rr_y=-half_w; 			//y coordinates of wheels


//other variables

double small_r; 		// shortest desired distance between key and center of rotation
				

double r, r_angle, rx, ry;	// length of radius to center of rotation,
				// and its angle and coordinates
						
double r_lf, r_lr, r_rf, r_rr; 	// distances from center of rotation to wheels
double r_max;			// max of the r's

double angle_in_radians;

double angle1, angle2;		//some temporaries for angles

   
    if (fabs(rotation) < almost_0)
    {
        // no rotation
		
		m_chassis->servo_lf.SetAngle(angle);
		m_chassis->servo_rf.SetAngle(angle);
		m_chassis->servo_lr.SetAngle(angle);
		m_chassis->servo_rr.SetAngle(angle);
		
		m_chassis->motor_lf.SetSpeed(speed);
		m_chassis->motor_lr.SetSpeed(speed);
		m_chassis->motor_rf.SetSpeed(speed);
		m_chassis->motor_rr.SetSpeed(speed);
		
	}
	else
	{
		// we need some rotation code here... 


// determine smallest r
// it should be at least as big as the length from the key point to any of the wheels

small_r = max(key_d, length-key_d);	//just a temp val here, to use in next line
small_r = sqrt( small_r*small_r + width*width/4.) + fudge; // add in the fudge factor


// compute r based on magnitude of twist or "rotation", between 0 and 1
// a previous step already took care of rotation too close to 0

// this is linear, but alternative functions could be tried
// they should be smoothly decreasing as rotation increases, and 
// be approximately =big_r for rotation close to 0,
// and be =small_r for rotation equal 1

r = big_r+(small_r-big_r)*fabs(rotation);

// calc r's coordinates
// positive rotation means clockwise,
// so center of rotation is to the right if you're facing in heading angle direction

angle_in_radians = angle*to_radians;

r_angle = rotation>0 ? angle_in_radians - half_pi : angle_in_radians + half_pi; 
rx = r*cos (r_angle);
ry = r*sin (r_angle);

// calc distance from center of rotation to each wheel
r_lf = sqrt((lf_x-rx)*(lf_x-rx) + (lf_y-ry)*(lf_y-ry));
r_lr = sqrt((lr_x-rx)*(lr_x-rx) + (lr_y-ry)*(lr_y-ry));
r_rf = sqrt((rf_x-rx)*(rf_x-rx) + (rf_y-ry)*(rf_y-ry));
r_rr = sqrt((rr_x-rx)*(rr_x-rx) + (rr_y-ry)*(rr_y-ry));

// find the biggest r, the distance of the wheel furthest away from the center of rotation

	r_max = max(max(r_lf, r_lr), max(r_rf,  r_rr));


// set wheel speeds
// the furthest wheel gets the full speed
// the rest are proportionally less

		m_chassis->motor_lf.SetSpeed(speed*r_lf/r_max);
		m_chassis->motor_lr.SetSpeed(speed*r_lr/r_max);
		m_chassis->motor_rf.SetSpeed(speed*r_rf/r_max);
		m_chassis->motor_rr.SetSpeed(speed*r_rr/r_max);


// compute and set angles
// use arctan of slope of line perpendicular
// to the line connecting the center of rotation and the wheel, and
// picking out of the arctan results, which could be + or - 180 degrees,
// the one closest to the input angle

if (fabs(ry-lf_y)<almost_0)
	if (rx-lf_x >=0) m_chassis->servo_lf.SetAngle(0); // angle is along x axis
		else m_chassis->servo_lf.SetAngle(180.) ; // but neg direction

	else { 	angle1 = (atan ((rx-lf_x)/(lf_y-ry)))*to_degrees;
		angle2 = angle1>0 ? angle1-180. :angle1+180.;

		m_chassis->servo_lf.SetAngle(fabs(angle1-angle) < fabs(angle1-angle) ? angle1 : angle2);
		}		

if (fabs(ry-lr_y)<almost_0)
	if (rx-lr_x >=0) m_chassis->servo_lr.SetAngle(0); // angle is along x axis
		else m_chassis->servo_lr.SetAngle(180.) ; // but neg direction

	else { 	angle1 = (atan ((rx-lr_x)/(lr_y-ry)))*to_degrees;
		angle2 = angle1>0 ? angle1-180. :angle1+180.;

		m_chassis->servo_lr.SetAngle(fabs(angle1-angle) < fabs(angle1-angle) ? angle1 : angle2);
		}	

if (fabs(ry-rf_y)<almost_0)
	if (rx-rf_x >=0) m_chassis->servo_rf.SetAngle(0); // angle is along x axis
		else m_chassis->servo_rf.SetAngle(180.);  // but neg direction

	else { 	angle1 = (atan ((rx-rf_x)/(rf_y-ry)))*to_degrees;
		angle2 = angle1>0 ? angle1-180. :angle1+180.;

		m_chassis->servo_rf.SetAngle(fabs(angle1-angle) < fabs(angle1-angle) ? angle1 : angle2);
		}	

if (fabs(ry-rr_y)<almost_0)
	if (rx-rr_x >=0) m_chassis->servo_rr.SetAngle(0); // angle is along x axis
		else m_chassis->servo_rr.SetAngle(180.) ; // but neg direction

	else { 	angle1 = (atan ((rx-rr_x)/(rr_y-ry)))*to_degrees;
		angle2 = angle1>0 ? angle1-180. :angle1+180.;

		m_chassis->servo_rr.SetAngle(fabs(angle1-angle) < fabs(angle1-angle) ? angle1 : angle2);
		}	

    }


	



	
}

void SwerveDrive::Stop()
{
	/// @todo: turn the wheels against the current
	/// acceleration vector and stop all motors. Is that actually
	/// a good way to do it?
	
	// if the vector is near zero, then just spread all fours 
	// and hope for the best

	m_chassis->servo_lf.SetAngle(45);
	m_chassis->servo_lr.SetAngle(315);
	m_chassis->servo_rf.SetAngle(135);
	m_chassis->servo_rr.SetAngle(225);
	
	m_chassis->motor_lf.SetSpeed(0);
	m_chassis->motor_lr.SetSpeed(0);
	m_chassis->motor_rf.SetSpeed(0);
	m_chassis->motor_rr.SetSpeed(0);
}










