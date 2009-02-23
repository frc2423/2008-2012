

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#ifdef USE_MSVC_MEMORY_TRACING
	// this enables advanced memory leak detection
	#include <wx/msw/msvcrt.h>
#endif


#include "SimulationWindow.h"

class MyApp : public wxApp
{
public:
	virtual bool OnInit();

	void OnClose(wxCloseEvent &event);

private:

};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	// initialize XRC
	wxXmlResource::Get()->InitAllHandlers();
    wxXmlResource::Get()->Load(wxT("ProductionCodeTester.xrc"));

	// create the window
	SimulationWindow * simulationWindow = new SimulationWindow(NULL);
	simulationWindow->Show();

	// hook the close event and we're done
	//simulationWindow->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MyApp::OnClose), NULL, this);

	return true;
}

void MyApp::OnClose(wxCloseEvent &event)
{
	ExitMainLoop();
}

