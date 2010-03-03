#include "MainFrame.h"
#include <string>
#include <fstream>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include "lang/lang.h"
#include "DisplayManager.h"
#include "FormatProvider.h"
#include "FileDataSource.h"
#include "AboutFrame.h"

using std::string;
using std::ifstream;



MainFrame* MainFrame::getInstance()
{
	static MainFrame* inst = new MainFrame(NULL);
	return inst;
}


MainFrame::MainFrame( wxWindow* parent )
		: MainFramePrototype( parent ), displayer(NULL)
{
	SetTitle(LangGet("MainFrame_title"));

	menuBar->SetMenuLabel(0, LangGet("MainFrame_fileMenu_label"));
	menuBar->SetMenuLabel(1, LangGet("MainFrame_helpMenu_label"));
	//menuBar->SetMenuLabel(1, LangGet("MainFrame_txdMenu_label"));
	//menuBar->SetMenuLabel(2, LangGet("MainFrame_imgMenu_label"));
	openItem->SetItemLabel(LangGet("MainFrame_openItem_label"));
	closeItem->SetItemLabel(LangGet("MainFrame_closeItem_label"));
	aboutItem->SetItemLabel(LangGet("MainFrame_aboutItem_label"));
	//txdExtractItem->SetItemLabel(LangGet("MainFrame_txdExtractItem_label"));
}


void MainFrame::onOpen( wxCommandEvent& event )
{
	wxString wildcards;

	FormatProvider** providers = DisplayManager::getInstance()->getFormatProviders();

	int j = 0;

	for (int i = 0 ; i < DisplayManager::getInstance()->getFormatProviderCount() ; i++) {
		if (providers[i]->isImplemented()) {
			if (j++ != 0) {
				wildcards.append(wxT("|"));
			}

			wxString wildcard = providers[i]->getFileWildcard();
			wildcards.append(wildcard);
		}
	}

	wxString path = wxFileSelector(LangGet("MainFrame_dlgOpenFile_title"),
			wxString(getenv("HOME"), wxConvUTF8), wxT(""), wxT(""), wildcards);

	if (!path.empty()) {
		displayFile(path);
	}
}


void MainFrame::onClose(wxCommandEvent& evt)
{
	closeItem->Enable(false);
	DisplayManager::getInstance()->closeDisplayer(displayer);
	displayer = NULL;
}


void MainFrame::displayFile(const wxString& filename)
{
	if (displayer != NULL) {
		DisplayManager::getInstance()->closeDisplayer(displayer);
		displayer = NULL;
	}

	FormatProvider* provider = DisplayManager::getInstance()->getFormatProvider(filename);
	displayer = provider->openDisplayer(this, new FileDataSource(filename));

	closeItem->Enable();
	sizer->Add(displayer, 1, wxEXPAND);
	Layout();
}


void MainFrame::onAbout(wxCommandEvent& evt)
{
	AboutFrame* frame = new AboutFrame(this);
	frame->Show(true);
}
