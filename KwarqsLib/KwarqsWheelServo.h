/**
	\file 		KwarqsWheelServo.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_WHEEL_SERVO_H
#define KWARQS_WHEEL_SERVO_H

// define this to enable auto-calibrate mode
#define AUTOCALIBRATE_SERVO


/**
	\class KwarqsWheelServo
	\brief Controls the direction of the wheel, relative to the robot
	
	Ideally, this will be controlled by a PID loop just like a real servo
	would be controlled. Will need at least two variants of this class,
	since the front and rear motors are going to be different types so
	they will need different tuning.
	
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
		bool invert_encoder,
		float param_P
	);
	
	virtual ~KwarqsWheelServo();

	/// enabled by default
	void Enable();
	void Disable();
	
	void Reset();
	
	/// Call this to tell the servo to automatically try to calibrate
	void AutoCalibrate();
	
	/// lets you know if the servo is currently calibrated or not
	bool IsCalibrated() { return !m_calibrating; }
	
	
	
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
	
	int GetRawEncoder()
	{
		return m_encoder.GetRaw();
	}
	
	void SetRawMotor(float value)
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
	
	DISALLOW_COPY_AND_ASSIGN(KwarqsWheelServo);
	
	// called when calibration is finished
#ifdef AUTOCALIBRATE_SERVO
	static void CalibrationIrqHandler(tNIRIO_u32 x, void * param);
#endif

	void CalibrationComplete();

	PIDController * m_pidController;
	
	
	Encoder 		m_encoder;
	
	// calibration sensor
	DigitalInput 	m_sensor;
	
	double 		m_outputScale;
	double		m_setAngle;
	
	int			m_encoderResolution;
	
	bool		m_calibrating;
	int			m_calibrated_offset;		// subtract from real counts
	double		m_calibrating_offset;		// how many degrees off is the calibration point?
	
	float		m_invert_motor;
	
	bool 		m_in_manual_mode;
	
	SEM_ID		m_calibration_mutex;
};

#endif
