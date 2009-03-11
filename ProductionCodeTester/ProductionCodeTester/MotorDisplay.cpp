/*
    WPILib Test Harness
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>

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

#include <wx/wx.h>

#include "MotorDisplay.h"

BEGIN_EVENT_TABLE(MotorDisplay, wxWindow)
	EVT_PAINT(MotorDisplay::OnPaint)
	EVT_ERASE_BACKGROUND(MotorDisplay::OnEraseBackground)
END_EVENT_TABLE()

MotorDisplay::MotorDisplay(wxWindow * parent, wxWindowID id) :
	m_value(0.0)
{
	Create(parent, id);
	SetMinSize(wxSize(25, 100));
}

void MotorDisplay::SetValue(double value)
{	
	if (value < -1.0)
		m_value = -1.0;
	else if (value > 1.0)
		m_value = 1.0;
	else
		m_value = value;
	
	UpdateDisplay();
}


void MotorDisplay::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	UpdateDisplay(&dc);
}

void MotorDisplay::OnEraseBackground(wxEraseEvent &event)
{
}

void MotorDisplay::UpdateDisplay(wxDC * dc)
{
	bool do_delete = false;

	if (dc == NULL)
	{
		dc = new wxClientDC(this);
		do_delete = true;
	}

	int w, h;
	GetClientSize(&w, &h);

	// fill background
	dc->SetPen(*wxBLACK_PEN);
	dc->SetBrush(*wxBLACK_BRUSH);
	dc->DrawRectangle(0, 0, w, h);
	
	// draw some lines
	//dc->SetPen(wxGREEN_PEN);
	//dc->SetPen(wxRED_PEN);
	
	// draw a center line
	dc->SetPen(*wxWHITE_PEN);
	dc->DrawLine(0, h/2, w, h/2);
	
	// then draw a line extending either up or down
	int one_fourth = w/4;
	int ht = (wxCoord)(((double)h * m_value)/2);
	
	if (m_value > 0)
	{
		dc->SetPen(*wxGREEN_PEN);
		dc->SetBrush(*wxGREEN_BRUSH);
		dc->DrawRectangle(one_fourth, h - (ht + h/2) , w/2, ht );
	}
	else if (m_value < 0)
	{
		dc->SetPen(*wxRED_PEN);
		dc->SetBrush(*wxRED_BRUSH);
		dc->DrawRectangle(one_fourth, h / 2, w - one_fourth*2, ht*-1);
	}
	
	if (do_delete)
		delete dc;
}


