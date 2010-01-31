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
#include <iostream>
#include <cstdio>
#include <wx/msgdlg.h>

using std::ifstream;
using std::cout;
using std::endl;

bool Application::OnInit() {
	ilInit();

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
