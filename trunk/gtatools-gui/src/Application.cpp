/*
 * Application.cpp
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#include "Application.h"
#include "TXDPanel.h"
#include "MainFrame.h"
#include <wx/frame.h>
#include <fstream>
#include <IL/il.h>

using std::ifstream;

bool Application::OnInit() {
	ilInit();

	/*wxFrame* frame = new wxFrame(NULL, -1, "Test Frame", wxDefaultPosition, wxSize(800, 600));
	TXDPanel* panel = new TXDPanel(frame);*/

	MainFrame* frame = new MainFrame(NULL);
	/*TXDPanel* panel = new TXDPanel(frame);

	ifstream* stream = new ifstream("bistro.txd", ifstream::in | ifstream::binary);
	TXDArchive* archive = new TXDArchive(stream);
	panel->displayArchive(archive);*/

	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

IMPLEMENT_APP(Application);
