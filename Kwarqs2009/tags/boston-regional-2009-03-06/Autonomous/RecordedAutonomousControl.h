/**
	\file 		RecordedAutonomousControl.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef RECORDED_AUTONOMOUS_CONTROL_H
#define RECORDED_AUTONOMOUS_CONTROL_H

#include "../KwarqsLib/KwarqsMovementControl.h"

#include "../DriveFilters/DriveRecorder.h"

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
	size_t m_idx;
	
	DriveRecorderData m_current_data;

	Notifier m_notifier;
	SEM_ID m_mutex;
};

#endif

