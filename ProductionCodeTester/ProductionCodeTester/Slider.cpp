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
#include "Slider.h"


BEGIN_EVENT_TABLE(Slider, wxWindow)
	EVT_PAINT(Slider::OnPaint)
	EVT_LEFT_DOWN(Slider::OnClick)
	EVT_KEY_DOWN(Slider::OnKeyDown)
	EVT_MOTION(Slider::OnMove)
	EVT_ERASE_BACKGROUND(Slider::OnEraseBackground)
END_EVENT_TABLE()

Slider::Slider(wxWindow * parent, wxWindowID id, double min, double max, bool readonly) :
	m_value(0),
	m_maxValue(max),
	m_minValue(min),
	m_readOnly(readonly)
{
	assert(min < max);

	AcceptsFocus();
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS);
	
	SetFont(*wxNORMAL_FONT);
	int x, y;
	
	GetTextExtent(wxT("0"), &x, &y);
	
	SetMinSize(wxSize(75, (y*3)/2));
	
	SetValue(0.0);
}

void Slider::SetReadOnly(bool readOnly)
{
	m_readOnly = readOnly;
}

void Slider::OnKeyDown(wxKeyEvent &event)
{
	if (wxWindow::FindFocus() != this)
		event.Skip();

	if (m_readOnly)
		return;

	switch (event.GetKeyCode())
	{
		case WXK_LEFT:
			SetValue( m_value - 0.01 );
			break;
		case WXK_RIGHT:
			SetValue( m_value + 0.01 );
			break;
		default:
			event.Skip();
	}
}

void Slider::OnMove(wxMouseEvent &event)
{
	if (event.Dragging())
		OnClick(event);
}

void Slider::OnClick(wxMouseEvent &event)
{
	if (m_readOnly)
		return;

	SetFocus();

	// normalize the click point
	double click;
	wxSize sz = GetClientSize();

	double len = sz.x - 10;
	
	if (event.GetX() < 5)
		click = 0;
	else if (event.GetX() > sz.x - 5)
		click = sz.x - 5;
	else
		click = event.GetX() - 5;
	
	double value = m_minValue + (click / len) * (m_maxValue - m_minValue);

	SetValue(value);
}


void Slider::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	UpdateDisplay(&dc);
}

void Slider::OnEraseBackground(wxEraseEvent &event)
{
}

double Slider::GetValue()
{
	return m_value;
}

void Slider::SetValue(double value)
{	
	if (value < m_minValue)
		m_value = m_minValue;
	else if (value > m_maxValue)
		m_value = m_maxValue;
	else
		m_value = value;
		
	m_strValue = wxString::Format(wxT("%.2f"), m_value);
	
	UpdateDisplay();
}

void Slider::UpdateDisplay(wxDC * dc)
{
	bool do_delete = false;

	if (dc == NULL)
	{
		dc = new wxClientDC(this);
		do_delete = true;
	}

	int w, h;
	GetClientSize(&w, &h);

	// fill background with a system color
	dc->SetPen(wxPen(GetForegroundColour()));
	dc->SetBrush(wxBrush(GetBackgroundColour()));
	dc->SetFont(GetFont());
	
	dc->DrawRectangle(0, 0, w, h);
	
	// figure out where zero lives
	wxCoord zero = 0;

	if (m_minValue < 0)
	{
		if (m_maxValue < 0)
		{
			zero = w;
		}
		else
		{
			zero = -1*(m_minValue / (m_maxValue - m_minValue)) *  (double)w;
		}
	}
	
	if (m_value > 0)
	{
		dc->SetBrush(*wxGREEN_BRUSH);

		// the positive width available
		double pw = m_maxValue - (m_minValue < 0 ? 0 : m_minValue);
		dc->DrawRectangle( zero, 0, ((pw - (m_maxValue - m_value))/pw) * (w - zero)  , h );
	}
	else if (m_value < 0)
	{
		dc->SetBrush(*wxRED_BRUSH);

		// the negative width available
		double nw = -1* ( m_minValue - (m_maxValue > 0 ? 0 : m_maxValue));

		// use an integer here to guarantee accuracy
		wxCoord ww = ((nw + (m_minValue - m_value))/nw) * zero;

		dc->DrawRectangle( zero - ww, 0, ww , h );
	}
	else
	{
		dc->DrawLine(zero, 0, zero, h);
	}


	// draw text

	
	wxSize extent = dc->GetTextExtent(wxT("-0.00"));
	
	dc->DrawText(m_strValue, w - extent.x, (h - extent.y)/2);
		
	if (do_delete)
		delete dc;
}



