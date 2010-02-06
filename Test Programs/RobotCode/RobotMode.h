#ifndef ROBOTMODE.H
#define ROBOTMODE.H

#include <WPILib.h>

class RobotMode
{
public:
	RobotMode(const RobotResources&);
	
	void SetMode(int mode){m_mode = mode;};
	void SetNext(void){m_mode++;};
	void Current(void){mode();};
	
	

	
private:
	
	
	
	const Gyro m_gyro;
	const Encoder m_leftEncoder;
	const Encoder m_rightEncoder;
	const Joystick m_joystick;
	
	void mode(void);
	
	int m_mode;
	static int available_modes;
	
};