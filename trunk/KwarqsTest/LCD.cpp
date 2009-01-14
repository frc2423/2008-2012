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


#include <WPILib.h>
#include "LCD.h"

// todo: can only have one instance of this, need to
// change that around.. 
LCD::LCD(UINT32 slot, unsigned char address) :
	m_address(address),
	m_task("LCD", (FUNCPTR)LCD::TaskStartS)
{
	m_device = DigitalModule::GetInstance(slot)->GetI2C(address);

	wpi_assert(m_device);
	
	// create a semaphore for signalling (binary)
	m_signal = semBCreate(0, SEM_FULL);
	
	// create a mutex for access
	m_mutex = semBCreate(0, SEM_EMPTY);
	
	if (!m_task.Start((UINT32)this)) wpi_assert(0 && "could not launch lcd task");
}

LCD::~LCD()
{
	m_running = true;
	// signal
	semGive(m_signal);
}


void LCD::DisplayOn()
{
	SendControl(0x41);
}

void LCD::DisplayOff()
{
	SendControl(0x42);
}

/**
 * @param row	0-3 are valid ranges
 * @param col	0-19 are valid positions
 */
void LCD::SetCursor(int row, int col)
{
	wpi_assert(row >= 0 && row < 4);
	wpi_assert(col >= 0 && col < 20);
	
	char pos =	(row == 0 ? 0x00 :
				(row == 1 ? 0x40 :
				(row == 2 ? 0x14 : 0x54)));
				
	SendControl2(0x45, pos + col); 
}

void LCD::SetCursorHome()
{
	SendControl(0x46);
}

/**
 * @param underline		If true, the cursor is underlined
 */
void LCD::SetCursorUnderline(bool underline)
{
	SendControl( underline ? 0x47 : 0x48);
}

void LCD::MoveCursorLeft()
{
	SendControl( 0x49 );
}

void LCD::MoveCursorRight()
{
	SendControl( 0x4a );
}

void LCD::SetCursorBlinking(bool blink)
{
	SendControl( blink ? 0x4b : 0x4c );
}

void LCD::Backspace()
{
	SendControl(0x4e);
}

void LCD::ClearScreen()
{
	SendControl(0x51);
}

/**
 * @param contrast 	Permitted range is 1-50
 */
void LCD::SetContrast(int contrast)
{
	wpi_assert(contrast > 0 && contrast <= 50);
	
	SendControl2(0x52, contrast);
}

/**
 * @param brightness	Permitted range is 1-16
 */
void LCD::SetBrightness(int brightness)
{
	SendControl2(0x53, brightness);
}

// not quite sure what these do
void LCD::MoveDisplayLeft()
{
	SendControl(0x55);
}

void LCD::MoveDisplayRight()
{
	SendControl(0x56);
}

// string functions
void LCD::WriteString(const char *str)
{
	{
		Synchronized s(m_mutex);
		int i = 0;
		while (str[i++] != 0 && i <= LCD_MAX_CHARS)
			m_bytes.push_back(str[i-1]);
	}
	
	// signal that its time to go
	semGive(m_signal);
}

// max size to write is 4*20 bytes
void LCD::printf(const char * format, ...)
{
	char buf[LCD_MAX_CHARS + 1];
	
	va_list ap;
	va_start(ap, format);
	int sz = vsnprintf(buf, sizeof(buf), format, ap);
	
	if (sz > 0)
		WriteString(buf);
}

	
void LCD::SendControl(unsigned char control)
{
	{
		Synchronized s(m_mutex);
	
		m_bytes.push_back(0xfe);
		m_bytes.push_back(control);
	}

	// signal the writer
	semGive(m_signal);
}

void LCD::SendControl2(unsigned char control, unsigned char control2)
{
	{
		Synchronized s(m_mutex);
		
		m_bytes.push_back(0xfe);
		m_bytes.push_back(control);
		m_bytes.push_back(control2);
	}
	
	// signal the writer
	semGive(m_signal);
}

void LCD::TaskStart()
{
	vector <char> bytes;
	
	do
	{
		{
			Synchronized s(m_mutex);
			
			// get the bytes to be written
			if (!m_bytes.empty())
				bytes.swap(m_bytes);
		}
		
		if (!bytes.empty())
		{
			// todo: is this actually correct? how do we
			// send bytes?
			for (size_t i = 0; i < bytes.size(); ++i)
				m_device->Write(m_address, bytes[i]);
			
			bytes.clear();
		}
		
		// wait for signal
		if (m_running)
			semTake(m_signal, WAIT_FOREVER);
	
		// if running, continue
	} while (m_running);
}

