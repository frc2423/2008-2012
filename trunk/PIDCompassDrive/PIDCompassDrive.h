

#ifndef _KWARQS_PIDCOMPASSDRIVE_H
#define _KWARQS_PIDCOMPASSDRIVE_H

#define ONE_OVER_SQRT_TWO 0.707106781

#include <WPILib.h>


class PIDGyro : public PIDSource {
public:
	PIDGyro(Gyro * gyro) :
		m_gyro(gyro)
	{}
	
	// returns an angle from 0 to 360
	float PIDGet()
	{
		float angle = fmod(gyro->GetAngle(), 360);
		return angle < 0 ? 360 - angle : angle;
	}
	
private:
	Gyro * m_gyro;
};

class PIDDrive : public PIDOutput {
public:
	PIDDrive(RobotDrive * drive) :
		m_drive(drive)
	{}
	
	void SetSpeed(float speed) { m_speed = speed; }
	
	void PIDWrite(float output)
	{
		drive->Drive(m_speed, output);
	}
	
private:
	float m_speed;
};


class PIDCompassDrive {
public:
	
	PIDCompassDrive(Gyro * gyro, RobotDrive * drive) :
		m_controller(CONST_P, CONST_I, CONST_D),
		m_pidInput(gyro),
		m_pidOutput(drive)
	{
		m_controller->SetInput(&m_pidInput, 0, 360);
		m_controller->SetOutput(&m_pidOutput, -1, 1);
		m_controller->SetContinuous();
	}
	
	void Drive( Joystick * stick )
	{
		double speed = hypot(x, y) * ONE_OVER_SQRT_TWO;
		double heading = atan2( y , x ) * (180 / M_PI) + 90;
		
		m_controller->SetSetpoint(heading < 0 ? 360 - heading : heading);
		m_pidOutput->SetSpeed(speed);
	}
	
private:
	
	PIDController 	m_controller;
	PIDGyro 		m_pidInput;
	PIDDrive 		m_pidOutput;
};


#endif
