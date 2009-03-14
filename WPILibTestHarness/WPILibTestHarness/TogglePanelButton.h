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

#ifndef TOGGLEPANELBUTTON_H
#define TOGGLEPANELBUTTON_H

class TogglePanelButton : public wxControl {
public:

	TogglePanelButton(wxWindow * parent, wxWindowID id = wxID_ANY);

	bool GetValue();
	void SetValue(bool value);

	void SetReadOnly(bool readonly = true);
	void SetEnabled(bool enabled = true);

private:

	void OnClick(wxMouseEvent &event);

	void OnEraseBackground(wxEraseEvent &event);
	void OnPaint(wxPaintEvent &event);

	void UpdateDisplay(wxDC * dc);
	
	void UpdateCursor();

	bool m_value;
	bool m_readOnly;
	bool m_enabled;

	wxColour m_onColor;
	wxColour m_offColor;

	DECLARE_EVENT_TABLE()
};

#endif
