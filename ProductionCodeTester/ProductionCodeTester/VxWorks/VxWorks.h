
#ifndef VXWORKS_H
#define VXWORKS_H

// misc definitions
typedef int STATUS;
typedef void (*FUNCPTR)(void*);
typedef void (*tInterruptHandler)(void*);

#include "pstdint.h"

typedef uint64_t 	UINT64;
typedef int64_t		INT64;
typedef uint32_t	UINT32;
typedef int32_t		INT32;
typedef uint16_t	UINT16;
typedef int16_t		INT16;
typedef uint8_t		UINT8;
typedef int8_t		INT8;

typedef int			SEM_ID;

#include <cassert>
#define wpi_assert assert
#define wpi_fatal(x) 

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <algorithm>

static inline void bzero( void * s, size_t n ) { memset(s, n, 0); }


#endif
