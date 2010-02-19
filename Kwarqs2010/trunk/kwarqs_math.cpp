/* Stub definitions for libmath subpart of libstdc++. */

/* Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.

   This file is part of the GNU ISO C++ Library.  This library is free
   software; you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option)
   any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this library; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.

   As a special exception, you may use this file as part of a free software
   library without restriction.  Specifically, if other files instantiate
   templates or use macros or inline functions from this file, or you compile
   this file and link it with other files to produce an executable, this
   file does not by itself cause the resulting executable to be covered by
   the GNU General Public License.  This exception does not however
   invalidate any other reasons why the executable file might be covered by
   the GNU General Public License.  */

#include "math.h"

double __hypot(double x, double y)
{
	double s = fabs(x) + fabs(y);
	if (s == 0.0)
		return s;
	x /= s; y /= s;
	return s * sqrt(x * x + y * y);
}

/*
	The following two are NOT taken from libstdc++
*/

double angle_normalize( double angle )
{
	angle = fmod( angle, 360.0 );
	if (angle < 0.0)
		angle += 360.0;
		
	return angle;
}


double angle_distance( double angle1, double angle2 )
{
	double error1 = angle_normalize(angle1 - angle2);
	double error2 = angle_normalize(angle2 - angle1);

	if (error1 < error2)
		return error1;
	else
		return error2;
}



