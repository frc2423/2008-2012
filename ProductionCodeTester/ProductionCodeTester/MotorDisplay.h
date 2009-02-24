
#ifndef MOTOR_DISPLAY_H
#define MOTOR_DISPLAY_H

class MotorDisplay : public wxControl {
public:

	MotorDisplay(wxWindow * parent, wxWindowID id = wxID_ANY);

	void SetValue(double value);

private:

	MotorDisplay();
	
	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground(wxEraseEvent &event);
	void UpdateDisplay(wxDC * dc = NULL);

	double m_value;

	DECLARE_EVENT_TABLE()
};

#endif
