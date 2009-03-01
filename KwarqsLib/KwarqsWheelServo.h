/**
	\file 		KwarqsWheelServo.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_WHEEL_SERVO_BASE_H
#define KWARQS_WHEEL_SERVO_BASE_H


/**
	\class KwarqsWheelServo
	\brief Controls the direction of the wheel, relative to the robot
	
	Ideally, this will be controlled by a PID loop just like a real servo
	would be controlled. Will need at least two variants of this class,
	since the front and rear motors are going to be different types so
	they will need different tuning.
	
	At the moment, this class is set to auto-calibrate the wheel
	as soon as it turns on. There is also a calibration function.
	
	Only a KwarqsDriveBase derived class should create these. 
*/
class KwarqsWheelServo : public PIDSource, public PIDOutput {
public:

	/// provided for status purposes
	static int UncalibratedServoCount();
	
	KwarqsWheelServo(
		UINT32 slot,
		UINT32 pwm_port, 
		UINT32 encoder_port1, UINT32 encoder_port2,
		UINT32 cal_port,
		double cal_offset,
		double outputScale,
		int encoderResolution,
		bool invert_motor,
		bool invert_encoder
	);
	
	virtual ~KwarqsWheelServo();

	/// enabled by default
	void Enable();
	void Disable();
	
	void EnableManualCalibration();
	void DisableManualCalibration();
	
	/// only needs to be called if the servo needs to be recalibrated
	void Calibrate();
	
	/// lets you know if the servo is currently calibrated or not
	bool IsCalibrated() { return m_calibrating; }
	
	void Reset() { CalibrationComplete(); }
	
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
	
	/// returns true if the wheel is at the sensor, false if not
	bool GetSensor();
	
	int GetRaw()
	{
		return m_encoder.GetRaw();
	}
	
	void SetRaw(float value)
	{
		Disable();
		m_motor.Set(value);
	}

	
	// only accessed by the PIDController class
	void PIDWrite(float output);
	double PIDGet();
	
	// used to tune the class
	void TuneParameters(float p, float i, float d);
	
	Victor 			m_motor;
	
private:
	
	static int m_uncalibrated_servos;
	
	// called when calibration is finished
	static void CalibrationIrqHandler(tNIRIO_u32 x, void * param);
	void CalibrationComplete();

	PIDController * m_pidController;
	
	
	Encoder 		m_encoder;
	
	// calibration sensor
	DigitalInput 	m_sensor;
	
	double 		m_outputScale;
	
	int			m_encoderResolution;
	
	bool		m_calibrating;
	int			m_calibrated_offset;		// subtract from real counts
	double		m_calibrating_offset;		// how many degrees off is the calibration point?
	
	float		m_invert_motor;
	
	bool 		m_in_manual_mode;
	
	SEM_ID		m_calibration_mutex;
};

#endif
