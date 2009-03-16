
#ifndef NIRIO_H
#define NIRIO_H

#include <VxWorks.h>

// none of these are really used at all.. 
typedef UINT32		tNIRIO_u32;
typedef int			tRioStatusCode;
typedef int			tAccumulator;

typedef void (*tInterruptHandler)(tNIRIO_u32 interruptAssertedMask, void *param);

#endif
