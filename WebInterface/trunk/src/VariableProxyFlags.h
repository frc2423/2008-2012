/*
    WebInterface
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>
	
	$Id$

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License v3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WI_VARIABLEPROXYFLAGS_H
#define WI_VARIABLEPROXYFLAGS_H

	
#define FN(varname, flag, T) 					\
	varname(T val)								\
	{											\
		varname##_ = val;						\
		fields_initialized |= flag;				\
		return *this;							\
	}


/**
	numeric data flags, it is setup where you can do a nice and verbose
	initialization of the type
*/
template <typename T>
struct NumericProxyFlagsImpl {

	typedef NumericProxyFlagsImpl<T> this_type;

	T 	default_value_;
	T	minval_;
	T	maxval_;
	T	step_;
	int	precision_;

	// various setters for the flags
	this_type& FN(default_value, 1, T)
	this_type& FN(minval, 2, T)
	this_type& FN(maxval, 4, T)
	this_type& FN(step, 8, T)
	this_type& FN(precision, 0, int)
	
	NumericProxyFlagsImpl();
	
	bool is_initialized() const 
	{
		return fields_initialized == 0x0F;
	}
		
private:
	int fields_initialized;
};

// integer specialization
template <>
inline
NumericProxyFlagsImpl<int>::NumericProxyFlagsImpl() :
	fields_initialized(0),
	precision_(0)
{}

// float specialization
template <>
inline
NumericProxyFlagsImpl<float>::NumericProxyFlagsImpl() :
	fields_initialized(0),
	precision_(2)
{}


/// definition for integers
typedef NumericProxyFlagsImpl<int>		IntProxyFlags;

/// definition for floats
typedef NumericProxyFlagsImpl<float>	FloatProxyFlags;


#undef FN

#endif
