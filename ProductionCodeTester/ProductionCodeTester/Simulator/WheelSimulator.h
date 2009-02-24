
#ifndef SIMULATOR_WHEEL_SIMULATOR_H
#define SIMULATOR_WHEEL_SIMULATOR_H

class WheelSimulator {
public:

	// pass in necessary stuff
	WheelSimulator(WheelInfo * info);

	void Step();

private:

	WheelInfo * info;

};

#endif
