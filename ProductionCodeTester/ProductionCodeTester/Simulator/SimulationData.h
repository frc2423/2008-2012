

#ifndef SIMULATION_SIMULATION_DATA_H
#define SIMULATION_SIMULATION_DATA_H

struct WheelInfo 
{
	double driveMotorSpeed;
	double servoMotorSpeed;
	double servoAngle;
	
	WheelInfo() :
		driveMotorSpeed(0.0), servoMotorSpeed(0.0), servoAngle(0.0)
	{}
};

struct SimulationData
{
	// lcd information
	char lcdText[21 * 6 + 2];

	// wheel information
	WheelInfo lf_wheel, lr_wheel, rf_wheel, rr_wheel;
	
	double gyroAngle;
	
	// body information here
	
	// default constructor
	SimulationData() :
		gyroAngle(0.0)
	{
		memset(lcdText, ' ', sizeof(lcdText));
	}
};

#endif
