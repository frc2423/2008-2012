/*
    WPILib Test Harness
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

#include <wx/wx.h>

#include "TogglePanelButton.h"

BEGIN_EVENT_TABLE(TogglePanelButton, wxControl)
	EVT_LEFT_UP(TogglePanelButton::OnClick)	
	EVT_ERASE_BACKGROUND(TogglePanelButton::OnEraseBackground)
	EVT_PAINT(TogglePanelButton::OnPaint)
END_EVENT_TABLE()


TogglePanelButton::TogglePanelButton(wxWindow * parent, wxWindowID id ) :
	m_value(false),
	m_readOnly(false),
	m_enabled(true),
	m_onColor(*wxGREEN),
	m_offColor(wxT("FOREST GREEN"))
{
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	
	SetFont(*wxNORMAL_FONT);
	int x, y;
	GetTextExtent(wxT("0"), &x, &y);

	SetMinSize(wxSize(y,y));
}


bool TogglePanelButton::GetValue()
{
	return m_value;
}

void TogglePanelButton::SetValue(bool value)
{
	if (value != m_value)
	{
		m_value = value;
		UpdateDisplay(NULL);
	}
}

void TogglePanelButton::SetReadOnly(bool readonly)
{
	if (m_readOnly != readonly)
	{
		m_readOnly = readonly;
		UpdateCursor();
	}
}

void TogglePanelButton::SetEnabled(bool enable)
{
	if (m_enabled != enable)
	{
		m_enabled = enable;
		
		UpdateDisplay(NULL);
		UpdateCursor();
	}
}

void TogglePanelButton::OnClick(wxMouseEvent &event)
{
	if (!m_readOnly)
		SetValue(!m_value);
}

void TogglePanelButton::OnEraseBackground(wxEraseEvent &event)
{
}

void TogglePanelButton::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	UpdateDisplay(&dc);
}

void TogglePanelButton::UpdateDisplay(wxDC * in)
{
	wxDC * dc = in;

	if (!dc)
		dc = new wxClientDC(this);

	// draw stuff
	int w, h;
	GetClientSize(&w, &h);

	// fill background with a system color
	dc->SetPen(wxPen(GetBackgroundColour()));
	dc->SetBrush(wxBrush(GetBackgroundColour()));
	
	dc->DrawRectangle(0, 0, w, h);

	dc->SetPen(*wxBLACK_PEN);

	if (!m_enabled)
		dc->SetBrush(*wxBLACK_BRUSH);
	else if (m_value)
		dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(m_onColor, wxSOLID));
	else
		dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(m_offColor, wxSOLID));

	dc->DrawCircle(w/2,h/2, w/2 < h/2 ? w/2 : h/2);

	if (!in)
		delete dc;
}

void TogglePanelButton::UpdateCursor()
{
	if (!m_enabled)
		SetCursor(wxCursor(wxCURSOR_NO_ENTRY));
	else if (m_readOnly)
		SetCursor(*wxSTANDARD_CURSOR);
	else
		SetCursor(wxCursor(wxCURSOR_HAND));
}

