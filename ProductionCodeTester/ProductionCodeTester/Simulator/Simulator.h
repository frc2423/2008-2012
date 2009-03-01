

#ifndef SIMULATOR_SIMULATOR_H
#define SIMULATOR_SIMULATOR_H

#include <vector>
#include <VxWorks/VxWorks.h>

struct FRCControlData;

class Encoder;
class PWM;
class Gyro;
class Notifier;
class DigitalInput;

struct ControlInterface;
class SimulationWindow;

struct EncoderInfo {
	Encoder * encoder;
	UINT32 slota, slotb;
	UINT32 channela, channelb;
	
	EncoderInfo(Encoder * e, UINT32 sa, UINT32 ca, UINT32 sb, UINT32 cb) :
		encoder(e), slota(sa), channela(ca), slotb(sb), channelb(cb)
	{}
};

struct PWMInfo {
	PWM * pwm;
	UINT32 slot;
	UINT32 channel;
	
	PWMInfo(PWM * p, UINT32 s, UINT32 c) :
		pwm(p), slot(s), channel(c)
	{}
};

struct GyroInfo {
	Gyro * gyro;
	UINT32 slot;
	UINT32 channel;
	
	GyroInfo(Gyro * g, UINT32 s, UINT32 c) :
		gyro(g), slot(s), channel(c)
	{}
};

struct DigitalInputInfo {
	DigitalInput * digitalInput;
	UINT32 slot;
	UINT32 channel;
	
	DigitalInputInfo(DigitalInput * di, UINT32 s, UINT32 c) :
		digitalInput(di), slot(s), channel(c)
	{}
};


/// provided to decouple the GUI related code from the simulation code
class Simulator {
public:

	// singleton instance of this class
	static Simulator * m_instance;	
	
	/// \name API For Simulation data
	/// @{
	
	static bool ShouldContinue();

	// step the simulation up a period of time (in seconds)
	static void Wait(double tm){ NextStep(tm); }
	static void NextStep(double tm) { m_instance->SimulateStep(tm); }

	// get time for WPILib
	static double GetTime() { 
		if (Simulator::m_instance)
			return Simulator::m_instance->m_time; 
		return 0;
	}
	
	// set LCD output data
	static void SetLCDData(const char * userDsLcdData, int userDsLcdDataLength);
	
	// retrieves control information
	static int GetControlData(FRCControlData *data, char *userData);
	
	static void AddEncoder(Encoder * e, UINT32 aSlot, UINT32 aChannel, UINT32 bSlot, UINT32 _bChannel);
	static void DeleteEncoder(Encoder * e);
	
	static void AddPWM(PWM * p, UINT32 slot, UINT32 channel);
	static void DeletePWM(PWM * p);
	
	static void AddGyro(Gyro * g, UINT32 slot, UINT32 channel);
	static void DeleteGyro(Gyro * g);
	
	static void AddNotifier(Notifier * n);
	static void DeleteNotifier(Notifier * n);
	
	static void AddDigitalInput(DigitalInput * di, UINT32 slot, UINT32 channel);
	static void DeleteDigitalInput(DigitalInput * di);
	
	/// @}
	
	/// \name Simulation functions
	/// @{
	
	static void StartSimulation(ControlInterface * controlInterface);
	
	/// @}
	
private:

	Simulator();
	Simulator(ControlInterface * controlInterface);
	
	void SimulateStep(double tm);
	
	void RouteHardware();

	std::vector <EncoderInfo> 		m_encoders;
	std::vector <PWMInfo> 			m_pwms;
	std::vector <GyroInfo> 			m_gyros;
	std::vector <Notifier*> 		m_notifiers;
	std::vector <DigitalInputInfo> 	m_digitalInputs;
	
	// current time
	double 							m_time;
	ControlInterface *				m_controlInterface;
	
	bool							m_hardware_routed;
	
	WheelSimulator					m_lf_wheel;
	WheelSimulator					m_lr_wheel;
	WheelSimulator					m_rf_wheel;
	WheelSimulator					m_rr_wheel;
};


#endif
