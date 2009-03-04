/**
	\file 		TelemetryData.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-04 03:28:27 -0500 (Wed, 04 Mar 2009) $
	\version 	$Rev: 265 $
*/


#ifndef TELEMETRYDATA_H
#define TELEMETRYDATA_H

#include "RobotChassis.h"
#include "KwarqsLib/PositionInformation.h"
#include "KwarqsLib/Serialization.h"



/// a struct to hold wheel data in it
struct WheelData {

	UINT32 servo_encoder_value;
	double servo_angle;
	double servo_setpoint;
	
	UINT32 motor_encoder_value;
	float motor_setpoint;
	
	WheelData(KwarqsWheelServo &servo, KwarqsWheelMotor &motor) :
		servo_encoder_value(servo.GetRawEncoder()),
		servo_angle(servo.GetCurrentAngle()),
		servo_setpoint(servo.GetSetAngle()),
		
		motor_encoder_value(motor.GetRawEncoder()),
		motor_setpoint(motor.GetRawMotor())
	{}
	
	explicit WheelData(const unsigned char * const buffer) :
	{}
	
	void Serialize(unsigned char * buffer) const
	{
		record<UINT32>::serialize(servo_encoder_value, 	buffer + se_off);
		record<double>::serialize(servo_angle, 			buffer + sa_off);
		record<double>::serialize(servo_setpoint, 		buffer + ss_off);
		record<UINT32>::serialize(motor_encoder_value, 	buffer + me_off);
		record<float>::serialize(motor_setpoint,		buffer + ms_off);
	}
	
	enum { 
		se_off = 0,
		sa_off = se_off + record<UINT32>::size,
		ss_off = sa_off + record<double>::size,
		me_off = ss_off + record<double>::size,
		ms_off = me_off + record<UINT32>::size,
		record_size = ms_off + record<float>::size
	};
};

/// a struct to hold position data in it
struct PositionData {

	double raw_angle;
	double field_angle;
	double ax, ay;
	
	PositionData(PositionInformation * info) :
		raw_angle(info->GetRawAngle()),
		field_angle(info->GetFieldAngle())
	{
		info->GetAcceleration(ax, ay);
	}
	
	void Serialize(unsigned char * buffer) const
	{
		record<double>::serialize(raw_angle, 	buffer);
		record<double>::serialize(field_angle, 	buffer + record<double>::size);
		record<double>::serialize(ax, 			buffer + record<double>::size*2);
		record<double>::serialize(ay, 			buffer + record<double>::size*3);
	}
	
	enum {
		record_size = record<double>::size * 4
	};
};

/// holds wheel and position data 
struct TelemetryData {

	double m_time;
	
	WheelData lf_wheel, lr_wheel, rf_wheel, rr_wheel;
	PositionData pdata;
	
	TelemetryData(
		RobotChassis &chassis,
		PositionInformation * info
	) :
		m_time(GetTime()),
		lf_wheel(chassis.servo_lf, chassis.motor_lf),
		lr_wheel(chassis.servo_lr, chassis.motor_lr),
		rf_wheel(chassis.servo_rf, chassis.motor_rf),
		rr_wheel(chassis.servo_rr, chassis.motor_rr),
		pdata(info)
	{}
		
		
	void Serialize(unsigned char * buffer) const
	{
		record<double>::serialize(m_time, buffer + tm_offset);
		record<WheelData>::serialize(lf_wheel, buffer + lf_offset);
		record<WheelData>::serialize(lr_wheel, buffer + lr_offset);
		record<WheelData>::serialize(rf_wheel, buffer + rf_offset);
		record<WheelData>::serialize(rr_wheel, buffer + rr_offset);
		record<PositionData>::serialize(pdata, buffer + pos_offset);
	}
	
	// serialization constants
	enum {
		tm_offset = 0,
		lf_offset = tm_offset + record<double>::size,
		lr_offset = lf_offset + record<WheelData>::size,
		rf_offset = lr_offset + record<WheelData>::size,
		rr_offset = rf_offset + record<WheelData>::size,
		pos_offset = rr_offset + record<WheelData>::size,
		
		record_size = pos_offset + record<PositionData>::size
	};
};

#endif
