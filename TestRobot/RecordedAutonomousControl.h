/**
	\file 		RecordedAutonomousControl.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-03-01 01:00:41 -0500 (Sun, 01 Mar 2009) $
	\version 	$Rev: 244 $
*/


#ifndef RECORDED_AUTONOMOUS_CONTROL_H
#define RECORDED_AUTONOMOUS_CONTROL_H

#include "DriveRecorder.h"

class RecordedAutonomousControl : public KwarqsMovementControl {
public:

	// constructor
	RecordedAutonomousControl(KwarqsDriveController * driveController);

	// required methods
	void Move();
	const char * Name() { return "RecordedAutonomousControl"; }
	
	void OnEnable();
	void OnDisable();
	
private:

	static void StaticTimerFn(void * param) { ((RecordedAutonomousControl*)param)->TimerFn(); }
	void TimerFn();

	std::vector<DriveRecorderData> m_data;
	size_t idx;
	
	DriveRecorderData m_current_data;

	Notifier m_notifier;
	SEM_ID m_mutex;
};

#endif

