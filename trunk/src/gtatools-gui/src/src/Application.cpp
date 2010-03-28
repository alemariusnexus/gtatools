/*
	Copyright 2010 David Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
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
