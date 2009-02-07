/*
 * Copyright (c) 2009 Dustin Spicuzza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Dustin Spicuzza nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DUSTIN SPICUZZA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DUSTIN SPICUZZA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
	Macros to make serialization easier
*/

#ifndef SDBX_SERIALIZATION_H
#define SDBX_SERIALIZATION_H

/// \name Macros for intrinsic types
/// @{


/// Generally, this macro is not preferred: but use it for converting types
/// from a buffer in a non-constructor
#define BUFFER_REF(type, offset)			*((type*)(buffer + (offset)))

/// Use this macro to deserialize a type from a buffer in a serialization constructor
#define DESERIALIZE(type, offset, var ) 	var(BUFFER_REF(type, offset))

/// Use this macro to serialize a type into a buffer in a Serialize() call
#define SERIALIZE(type, offset, var ) 		BUFFER_REF(type, offset) = var

/// @}


/// \name Macros for types supporting the generic serialization mechanism types
/// @{


/// Use this macro to deserialize a type from a buffer in a serialization constructor
#define DESERIALIZE_G(type, offset, var) \
	var(record<type>::deserialize(buffer + (offset)))
	
/// Use this macro to serialize a type into a buffer in a Serialize() call
#define SERIALIZE_G(type, offset, var) \
	record<type>::serialize(var, buffer + (offset));

/// @}
	

/**
	\struct record
	\brief Generic serialization class

	Read the notes in SerializedFile.h first

	These allow us to serialize/deserialize anything that we want without 
	knowing whether	its an intrinsic or a user-defined class. There is a 
	template defined, record<T>, that provides three pieces of
	functionality:
	
		record<T>::size						The record_size of the type
		
		record<T>::deserialize(buffer)		Used in an initializer of a 
											constructor
											
		record<T>::serialize(var, buffer)	Used to serialize a type to a
											buffer.
	
	As always, an example is the best way to explain how to use it:
	
		template <typename T1, typename T2>
		struct test_record {
			enum { record_size =	record<T1>::size +
									record<T2>::size};

			test_record(const unsigned char * const buffer) :
				value1(record<T1>::deserialize(buffer)),
				value2(record<T2>::deserialize(buffer + record<T1>::size))
			{}
			
			void Serialize(unsigned char * buffer) const
			{
				record<T1>::serialize(value1, buffer);
				record<T2>::serialize(value2, buffer + record<T1>::size);
			}

			T1 value1;
			T2 value2;
		};
	
	As you can see, its pretty straightforward. The compiler should inline
	the extra function calls when you're using optimizations, so there 
	shouldn't be any extra overhead. However I would anticipate that 
	compiling without optimizations would result in some overhead.

*/


// this struct works with any user defined type that conforms to the
// serialization concept as defined in SerializedFile.h 
//
// This doesn't work with built in types, but there are specializations
// defined below which work for those.
template <class T>
struct record {
	enum { size = T::record_size };
	
	static inline 
	const unsigned char * const deserialize(const unsigned char * const buffer) 
	{ 
		return buffer; 
	}
	
	static inline 
	void serialize(const T &var, unsigned char * buffer) 
	{ 
		var.Serialize(buffer);
	}
};

// specialize for pointers in a braindead way.. but at least it
// will compile
template <class T>
struct record <T *> {
	enum { size = 0 };
	
	static inline 
	T * deserialize(const unsigned char * const buffer) 
	{ 
		assert(0 && "you should not serialize pointers");
		return NULL;
	}
};




// used to define specializations for intrinsic types
#define RECORD_DEFS(T) 										\
template <>													\
struct record<T> {											\
	enum { size = sizeof(T) };								\
															\
	static inline 											\
	T deserialize(const unsigned char * const buffer) 		\
	{ 														\
		return *((T*)buffer);								\
	}														\
															\
	static inline 											\
	void serialize(const T &var, unsigned char *  buffer) 	\
	{														\
		*((T*)buffer) = var;								\
	}														\
};

// this probably takes care of all the intrinsics.. 
RECORD_DEFS(char);
RECORD_DEFS(unsigned char);
RECORD_DEFS(short int);
RECORD_DEFS(unsigned short int);
RECORD_DEFS(int);
RECORD_DEFS(unsigned int);
RECORD_DEFS(long);
RECORD_DEFS(unsigned long);
RECORD_DEFS(long long);
RECORD_DEFS(unsigned long long);

RECORD_DEFS(float);
RECORD_DEFS(double);


#undef RECORD_DEFS



#endif


