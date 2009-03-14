
#ifndef VXWORKS_H
#define VXWORKS_H

// misc definitions
typedef int STATUS;
typedef void (*FUNCPTR)(void*);


// this sucks

#ifdef _MSC_VER

typedef unsigned __int64		UINT64;
typedef signed __int64			INT64;
typedef unsigned int			UINT32;
typedef signed int				INT32;
typedef unsigned short			UINT16;
typedef signed short			INT16;
typedef unsigned char			UINT8;
typedef signed char				INT8;

#endif

typedef int			SEM_ID;

#include <cassert>
#define wpi_assert assert
#define wpi_fatal(x) 

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>

static inline void bzero( void * s, size_t n ) { memset(s, n, 0); }


#endif
