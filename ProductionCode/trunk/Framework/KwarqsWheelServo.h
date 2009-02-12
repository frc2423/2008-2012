/**
	\file 		KwarqsWheelServo.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_WHEEL_SERVO_BASE_H
#define KWARQS_WHEEL_SERVO_BASE_H

#include "TunablePIDController.h"

template <int i>
class UglyServoHack;


/**
	\class KwarqsWheelServo
	\brief Controls the direction of the wheel, relative to the robot
	
	This is controlled by a PID loop just like a real servo
	would be controlled. There are a number of different configuration
	parameters required for each motor, all which are set in the constructor.
	
	At the moment, this class is set to auto-calibrate the wheel as soon
	as it turns on (open question on whether this is necessary). There 
	is also a calibration function. Ideally, the calibration point will be
	close to straight ahead.
	
	Only a KwarqsDriveBase derived class should create these. 
*/
class KwarqsWheelServo : public PIDSource, public PIDOutput {
public:

	KwarqsWheelServo(
		UINT32 slot,
		UINT32 pwm_port, 
		UINT32 encoder_port1, UINT32 encoder_port2,
		UINT32 cal_port,
		double outputScale,
		int encoderResolution,
		double cal_offset
	);
	
	virtual ~KwarqsWheelServo();

	/// only needs to be called if the servo needs to be recalibrated
	void Calibrate();
	
	/// lets you know if the servo is currently calibrated or not
	bool IsCalibrated() { return m_calibrating; }
	
	/// Set the angle that the wheel should be pointing, where
	/// 0 is straight ahead and angle increments positively 
	/// counter clockwise
	void SetAngle(double angle);
	
	/// Get the angle that the wheel is supposed to be pointing, where
	/// 0 is straight ahead and angle increments positively 
	/// counter clockwise
	double GetSetAngle();
	
	/// Get the angle that the wheel is actually pointing, where
	/// 0 is straight ahead and angle increments positively 
	/// counter clockwise
	double GetCurrentAngle();

	
	// only accessed by the PIDController class
	void PIDWrite(float output);
	double PIDGet();
	
	// used to tune the class
	void TuneParameters(float p, float i, float d);
	
private:
	
	// called when calibration is finished
	void CalibrationComplete();

	TunablePIDController * m_pidController;
	
	Victor 			m_motor;
	Encoder 		m_encoder;
	
	// calibration sensor
	DigitalInput 	m_sensor;
	
	double 		m_outputScale;
	
	int			m_encoderResolution;
	
	
	bool		m_calibrating;
	int			m_calibrated_offset;		// subtract from real counts
	double		m_calibrating_offset;		// how many degrees off is the calibration point?
	
	template <int i>
	friend class UglyServoHack;
};



// ignore this, its not important. Really, just ignore it. Go away.
template <int i>
class UglyServoHack {
public:
	static void Handler(tNIRIO_u32 x, void * param)
	{
		UglyServoHack<i>::that->CalibrationComplete();
		UglyServoHack<i>::that = NULL;
	}
	
	static KwarqsWheelServo * that;
};

#define SUSH(N) 								\
	if (UglyServoHack<N>::that == NULL)			\
	{											\
		UglyServoHack<N>::that = this;			\
		handler = UglyServoHack<1>::Handler;	\
	}

#define SETUP_UGLY_SERVO_HACK						\
	SUSH(1) else SUSH(2) else SUSH(3) else SUSH(4) 	\
	else {											\
		wpi_assert(0 && "too many servos created");	\
	}

#endif
