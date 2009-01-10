/*
       Simple implementation of RobotDrive abstract class

       Bonus points: determine why this is called an 'abstract' class
*/

#ifndef _ROBOTDRIVE_H
#define _ROBOTDRIVE_H

class RobotDrive {
public:

      virtual void Drive(float robot_Compass, float joystick_x, float joystick_y, float joystick2_y) = 0;

};


#endif

