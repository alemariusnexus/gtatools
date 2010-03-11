/*
 * Application.cpp
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#include "lang/lang.h"
#include "Application.h"
#include "MainFrame.h"
#include <wx/frame.h>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include <wx/image.h>

using std::ifstream;
using std::cout;
using std::endl;



bool Application::OnInit()
{
	if (!wxApp::OnInit()) {
		return false;
	}

	wxInitAllImageHandlers();

	MainFrame* frame = MainFrame::getInstance();

	if (!fileToOpen.empty()) {
		frame->displayFile(fileToOpen);
	}

	frame->Show(true);
	SetTopWindow(frame);
	return true;
}


void Application::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.AddParam(wxT("The file to open"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}


bool Application::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (parser.GetParamCount() > 0) {
		fileToOpen = parser.GetParam(0);
	} else {
		fileToOpen = wxT("");
	}

	return true;
}

IMPLEMENT_APP(Application);
