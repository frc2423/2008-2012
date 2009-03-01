/**
	\file 		DriveRecorder.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-01 01:00:41 -0500 (Sun, 01 Mar 2009) $
	\version 	$Rev: 244 $
*/


#ifndef DRIVE_RECORDER_H
#define DRIVE_RECORDER_H

#include "KwarqsLib/DelayTime.h"
#include "KwarqsLib/DataFile.h"

#include <vector>


#define DRIVERECORDER_PERIOD 0.025

struct DriveRecorderData {
	double speed;
	double angle;
	double rotation;
	bool stop;
	
	bool DoWriteWith(DataFile &file)
	{
		return 
			file.Write(speed) &&
			file.Write(angle) &&
			file.Write(rotation) &&
			file.Write(stop);
	}
	
	bool DoReadWith(DataFile &file)
	{
		return
			file.Read(speed) && 
			file.Read(angle) &&
			file.Read(rotation) &&
			file.Read(stop)
	}
	
	void Reset()
	{
		speed = 0;
		angle = 0;
		rotation = 0;
		stop = false;
	}
};

/**
	\class DriveRecorder
*/
class DriveRecorder : public KwarqsDriveBase 
{
	void Disable(){}
	void Enable(){}

	DriveRecorder();
	~DriveRecorder();
	
	/// this is a filter, so return false
	virtual bool IsPhysicalDrive() { return false; }

	/// does filtering stuff
	void Move(double &speed, double &angle, double &rotation, bool &stop);

	/// Return the name of the class
	const char * Name() { return "DriveRecorder"; }
	
private:

	DISALLOW_COPY_AND_ASSIGN(DriveRecorder)

	static void StaticTimerFn(void * param) { ((DriveRecorder*)param)->TimerFn(); }
	void TimerFn();
	
	void FlushDataToFile();
	
	// protects all variables
	SEM_ID m_mutex;
	
	KwarqsJoystick m_stick;
	DriverStation * m_ds;
	DriverStationLCD * lcd;
	Notifier * m_notifier;
	
	// true if in record mode (enabled means write to file)
	bool m_recording;
	bool m_enabled;
	
	// signals the end of record mode
	double m_record_mode_endtime;
	
	// lcd printout timer
	double m_lastupdate;
	
	
	// used to pass information to the notifier
	DriveRecorderData m_last_data;
	std::vector<DriveRecorderData> m_data;
	
	DataFile m_file;
	
	DelayTime m_lcdDelay;	
};

#endif
