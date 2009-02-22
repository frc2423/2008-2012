

#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H

struct FRCControlData;

class Encoder;
class PWM;
class Gyro;
class Notifier;
class DigitalInput;

class Simulator {
public:

	// step the simulation up a period of time (in seconds)
	static void Wait(double tm){}

	// get time for WPILib
	static double GetTime() { return 0; }
	
	// set LCD output data
	static void SetLCDData(const char * userDsLcdData, int userDsLcdDataLength){}
	
	// retrieves control information
	static int GetControlData(FRCControlData *data, char *userData) { return 0; }
	
	static void AddEncoder(
		Encoder * e, 
		UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel, 
		bool reverseDirection){}
	
	static void DeleteEncoder(Encoder * e){}
	
	static void AddPWM(PWM * p, UINT32 slot, UINT32 channel){}
	static void DeletePWM(PWM * p){}
	
	static void AddGyro(Gyro * g, UINT32 channel){}
	static void DeleteGyro(Gyro * g){}
	
	static void AddNotifier(Notifier * n){}
	static void DeleteNotifier(Notifier * n){}
	
	static void AddDigitalInput(DigitalInput * di, UINT32 channel){}
	static void DeleteDigitalInput(DigitalInput * di){}
	
};


#endif
