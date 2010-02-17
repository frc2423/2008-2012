#ifndef SAMPLEPIDOUTPUT_H
#define SAMPLEPIDOUTPUT_H

#include <WPILib.h>

#define MINIMUM_SCORE 0.01

class SamplePIDOutput : public PIDOutput {
public:
	SamplePIDOutput(RobotDrive& base) :
		m_base (base)
	{}

	void PIDWrite(float output) {
		m_base.ArcadeDrive(0.0, output);
	}
private:
	RobotDrive& m_base;
};

#endif
