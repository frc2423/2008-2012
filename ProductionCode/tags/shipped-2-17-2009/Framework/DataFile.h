/**
	\file 		DataFile.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

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

#ifndef FRAMEWORK_DATAFILE_H
#define FRAMEWORK_DATAFILE_H

#include <stdio.h>
#include <cassert>
#include "Serialization.h"

/**
	\class DataFile
	\brief A simple wrapper around fopen
	
	Contains serialization support so you can write arbitrary types to file
	provided they can be serialized (all intrinsics, and items that support
	the serialization pattern in Serialization.h). Also allows you to do 
	'normal' writing to file as well. 
*/
class DataFile {
public:

	/// the mode used to open the file with
	enum OpenMode {
		ReadOnly,
		ReadWrite,
		TruncateAndWrite,
		AppendAndWrite,
		ReadAndAppend
	};

	DataFile() :
		m_file(NULL)
	{}
	
	~DataFile()
	{
		Close();
	}
	
	/// close the file
	void Close()
	{
		if (m_file)
		{
			fclose(m_file);
			m_file = NULL;
		}
	}
	
	/// returns true if end of file has been reached
	bool Eof()
	{
		return feof(m_file);
	}
	
	/// opens a file with the specified mode
	bool Open(const char * filename, OpenMode mode)
	{
		Close();
		if (mode != ReadOnly)
			Priv_SetWriteFileAllowed(1);
		
		switch (mode)
		{
			case ReadOnly:
				m_file = fopen(filename, "r");
				break;
			case ReadWrite:
				m_file = fopen(filename, "r+");
				break;
			case TruncateAndWrite:
				m_file = fopen(filename, "w");
				break;
			case AppendAndWrite:
				m_file = fopen(filename, "w+");
				break;
			case ReadAndAppend:
				m_file = fopen(filename, "a+");
				break;
		}
		
		return m_file != NULL;
	}
	
	/// normal read
	bool Read(unsigned char * buffer, size_t sz)
	{
		return fread(buffer, sz, 1, m_file) == 1;
	}
	
	/// read serializable types
	template <typename T>
	bool Read(T &t)
	{
		unsigned char buffer[record<T>::size];
		if (!Read(buffer, record<T>::size))
			return false;
		
		t = T(record<T>::deserialize(buffer));
		return true;
	}
	
	/// normal write
	bool Write(const unsigned char * buffer, size_t sz)
	{
		return fwrite(buffer, sz, 1, m_file) == 1;
	}

	/// write serializable types
	template <typename T>
	bool Write(const T& t)
	{
		unsigned char buffer[record<T>::size];
		record<T>::serialize(t, buffer);
		
		return Write(buffer, record<T>::size);
	}
	
private:
	FILE * m_file;

};



#endif
