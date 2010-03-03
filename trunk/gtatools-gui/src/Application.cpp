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



bool Application::OnInit() {

	wxInitAllImageHandlers();

	MainFrame* frame = MainFrame::getInstance();

	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

IMPLEMENT_APP(Application);
