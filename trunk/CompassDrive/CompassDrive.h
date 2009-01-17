#include <WPILib.h>

class CompassDrive
{
public:

	CompassDrive( SpeedController * leftMotor, SpeedController * rightMotor); 
	 
    virtual void Drive(float robot_Compass, float joystick_x, float joystick_y, float joystick2_y);

    float oneJoystick(float joystick_x, float joystick_y);
	float angleChange(float robot_Compass, float joystick_x, float joystick_y);
    float turnRate(float robot_Compass, float joystick_x, float joystick_y, float speed);
    float tankLeftMotors(float angleChange, float turn_Rate, float speed);
    float tankRightMotors(float angleChange, float turn_Rate, float speed);
    
private:
  
    float controllerCompass(float joystick_x, float joystick_y);
    
    SpeedController * m_leftMotor;
    SpeedController * m_rightMotor;
    
};

