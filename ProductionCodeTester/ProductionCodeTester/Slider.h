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

#ifndef SLIDER_H
#define SLIDER_H

class Slider : public wxControl {
public:

	Slider(
		wxWindow * parent, 
		wxWindowID id = wxID_ANY, 
		double min = 0, 
		double max = 5, 
		bool readonly = false);

	void SetReadOnly(bool readonly = true);
	void SetMinMax(double min, double max);
	void SetValue(double value);
	double GetValue();

private:

	Slider();
	
	void OnKeyDown(wxKeyEvent &event);
	void OnMove(wxMouseEvent &event);
	void OnClick(wxMouseEvent &event);

	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground(wxEraseEvent &event);
	void UpdateDisplay(wxDC * dc = NULL);

	double m_maxValue;
	double m_minValue;
	double m_value;
	
	wxString m_strValue;

	bool m_readOnly;

	DECLARE_EVENT_TABLE()
};

#endif
