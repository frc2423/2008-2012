
#ifndef DATAADJUSTER_ALLOCATIONFLAGS_H
#define DATAADJUSTER_ALLOCATIONFLAGS_H

template <typename T>
struct AllocationFlags {

	T		max_value;
	T		min_value;
	T		step_size;

};

typedef AllocationFlags<int> IntAllocationFlags;


#endif
