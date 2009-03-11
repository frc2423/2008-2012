/*
    WPILib Test Harness
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License v3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SIMULATOR_SIMULATIONDATA_H
#define SIMULATOR_SIMULATIONDATA_H

#define ANALOG_SLOT_1 	1
#define ANALOG_SLOT_2 	2
#define DIGITAL_SLOT_1 	4
#define DIGITAL_SLOT_2 	6

#define DIGITAL_PWM_CHANNELS 	10
#define DIGITAL_IO_CHANNELS 	14


class Encoder;
class Gyro;
class PWM;
class DigitalInput;
class DigitalOutput;
class AnalogChannel;


struct EncoderInfo {
	Encoder * encoder;
	UINT32 slota, slotb;
	UINT32 channela, channelb;
	
	int count;
	
	EncoderInfo(Encoder * e, UINT32 sa, UINT32 ca, UINT32 sb, UINT32 cb) :
		encoder(e), slota(sa), channela(ca), slotb(sb), channelb(cb), count(0)
	{}
	
	void Transfer();
};



struct GyroInfo {
	Gyro *	gyro;
	UINT32	slot;
	UINT32	channel;
	
	float	angle;
	
	GyroInfo(Gyro * g, UINT32 s, UINT32 c) :
		gyro(g), slot(s), channel(c), angle(0.0)
	{}
	
	// only called by the simulation
	void Transfer();
};


struct DigitalIOData {
	DigitalInput * digitalInput;
	DigitalOutput * digitalOutput;

	bool used;
	bool value;
	
	DigitalIOData() : 
		digitalInput(NULL), digitalOutput(NULL), used(false), value(false)
	{}
	
	// only called by the simulation
	void Transfer();
};

struct PWMData {
	PWM * pwm;
	float speed;
	
	PWMData() : 
		pwm(NULL), speed(0.0)
	{}
	
	// only called by the simulation
	void Transfer();
};

struct RelayData {
	bool used;
	
	/// @todo implement this
	RelayData() : 
		used(false) 
	{}
	
	void Transfer();
};

struct DigitalModuleData {

	DigitalIOData 	io[14];
	PWMData 		pwm[10];
	RelayData		relay[8];
	
	// only called by the simulation
	void Transfer()
	{
		for (size_t i = 0; i < 14; i++)
			io[i].Transfer();
		for (size_t i = 0; i < 10; i++)
			pwm[i].Transfer();
		for (size_t i = 0; i < 8; i++)
			relay[i].Transfer();
	}
};

struct AnalogIOData {

	AnalogChannel * analogChannel;

	bool used;
	float value;	// 0-5v
	
	AnalogIOData() :
		used(false), value(0.0)
	{}
	
	// only called by the simulation
	void Transfer();
};

struct AnalogModuleData 
{
	AnalogIOData io[8];
	
	void Transfer();
};


// this is copied by value, so don't make it too obscene
struct SimulationData
{
	// lcd information
	char lcdText[21 * 6 + 2];

	// raw io
	AnalogModuleData 	analogModule[2];
	DigitalModuleData 	digitalModule[2];	
	
	// encoders, gyros
	vector<EncoderInfo> encoders;
	vector<GyroInfo>	gyros;
	
	
	// default constructor
	SimulationData() 
	{
		memset(lcdText, ' ', sizeof(lcdText));
	}
	
	// transfers the data to/from the simulation space
	void Transfer()
	{
		digitalModule[0].Transfer();
		digitalModule[1].Transfer();
		
		analogModule[0].Transfer();
		analogModule[1].Transfer();
		
		for (size_t i = 0; i < encoders.size(); i++)
			encoders[i].Transfer();
			
		for (size_t i = 0; i < gyros.size(); i++)
			gyros[i].Transfer();
	}
};

#endif
