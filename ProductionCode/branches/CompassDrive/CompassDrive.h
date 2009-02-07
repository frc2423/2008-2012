#include <WPILib.h>
#include "Framework/KwarqsDevices.h"
#include "Framework/KwarqsMovementControl.h"

class CompassDrive: public KwarqsMovementControl
{
public:
	
	float direction;
	bool buttonPrev;

	// required methods
	void Move();
	const char * Name() { return "CompassDrive"; }
	
	CompassDrive( KwarqsDriveController * driveController); 
	 
    float control(float turn_Joystick, float speed_Joystick);
    float turnRate(int turn_Type, float angleChange_H, float angleChange_L, float speed_H, float speed_L);
    void getVariables();
    
private:
	
	float joystick1_y, joystick2_y, joystick1_x, joystick2_x, robot_Compass;
	float get_Turn, speed;
	
    float controllerCompass(float joystick_x, float joystick_y);
	float angleChange(float robot_Compass, float joystick_x, float joystick_y);
    float Direction(bool button);
	
	Joystick * stick1;
	Gyro * gyro1;
};

