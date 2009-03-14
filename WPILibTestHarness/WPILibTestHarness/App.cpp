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
    

	// try and find the silly xrc file
	wxFileName file(wxGetCwd());
	file.SetFullName(wxT("WPILibTestHarness.xrc"));

	if (!file.IsFileReadable() || 
		!wxXmlResource::Get()->Load(file.GetFullPath()))
	{

	// try going up
	file.AppendDir(wxT("WPILibTestHarness"));

	if (!file.IsFileReadable() || 
		!wxXmlResource::Get()->Load(file.GetFullPath()))
	{

	// try going back a dir
	file.RemoveLastDir();
	file.RemoveLastDir();
	file.AppendDir(wxT("WPILibTestHarness"));

	if (!file.IsFileReadable() || 
		!wxXmlResource::Get()->Load(file.GetFullPath()))
	{		
		wxMessageBox(wxT("WPILibTestHarness.xrc was not found! This file must be in the current working directory or this program cannot execute!"), wxT("Error finding XRC file"));
		return false;
	}
	}
	}

	// create the window
	SimulationWindow * simulationWindow = new SimulationWindow(NULL);
	simulationWindow->Show();

	return true;
}

void MyApp::OnClose(wxCloseEvent &event)
{
	ExitMainLoop();
}

