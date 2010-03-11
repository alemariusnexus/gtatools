/*
 * Application.h
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#define wxUSE_LISTBOX 1
#define wxUSE_FILEDLG 1
#define wxUSE_DIRDLG 1
#define wxUSE_CHOICEDLG 1

#include "guiconfig.h"
#include <wx/app.h>
#include <wx/cmdline.h>

class Application : public wxApp
{
public:
	virtual bool OnInit();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
	wxString fileToOpen;
};

#endif /* APPLICATION_H_ */
