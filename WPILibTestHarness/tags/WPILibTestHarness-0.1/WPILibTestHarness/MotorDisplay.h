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

#ifndef MOTOR_DISPLAY_H
#define MOTOR_DISPLAY_H

class MotorDisplay : public wxControl {
public:

	MotorDisplay(wxWindow * parent, wxWindowID id = wxID_ANY);

	void SetReadOnly(bool readOnly);

	void SetValue(double value);

private:

	MotorDisplay();
	
	void OnMove(wxMouseEvent &event);
	void OnClick(wxMouseEvent &event);
	
	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground(wxEraseEvent &event);
	void UpdateDisplay(wxDC * dc = NULL);

	double	m_value;
	bool	m_readOnly;

	DECLARE_EVENT_TABLE()
};

#endif
