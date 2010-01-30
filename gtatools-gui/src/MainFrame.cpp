#include "MainFrame.h"
#include "TXDPanel.h"
#include "IMGPanel.h"

#include <TXDArchive.h>
#include <string>
#include <fstream>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include "lang/lang.h"
#include "DisplayManager.h"
#include "FormatProvider.h"

using std::string;
using std::ifstream;


MainFrame::MainFrame( wxWindow* parent )
		: MainFramePrototype( parent ), displayer(NULL)
{
	SetTitle(LangGet(MainFrame_title));

	menuBar->SetMenuLabel(0, LangGet(MainFrame_fileMenu_label));
	menuBar->SetMenuLabel(1, LangGet(MainFrame_txdMenu_label));
	menuBar->SetMenuLabel(2, LangGet(MainFrame_imgMenu_label));
	openItem->SetItemLabel(LangGet(MainFrame_openItem_label));
	closeItem->SetItemLabel(LangGet(MainFrame_closeItem_label));
	txdExtractItem->SetItemLabel(LangGet(MainFrame_txdExtractItem_label));
}


void MainFrame::onOpen( wxCommandEvent& event )
{
	wxString wildcards;

	FormatProvider** providers = DisplayManager::getInstance()->getFormatProviders();

	for (int i = 0 ; i < DisplayManager::getInstance()->getFormatProviderCount() ; i++) {
		if (i != 0) {
			wildcards.append("|");
		}

		wxString wildcard = providers[i]->getFileWildcard();
		wildcards.append(wildcard);
	}

	wxString path = wxFileSelector(LangGet(MainFrame_dlgOpenFile_title), getenv("HOME"), "", "",
			wildcards);

	if (!path.empty()) {
		displayFile(path);
	}
}


void MainFrame::onClose(wxCommandEvent& evt)
{
	closeItem->Enable(false);
	DisplayManager::getInstance()->closeDisplayer(displayer);
	//displayer->closeFile();
	//delete displayer;
	displayer = NULL;
}


void MainFrame::displayFile(const char* filename)
{
	if (displayer != NULL) {
		DisplayManager::getInstance()->closeDisplayer(displayer);
		displayer = NULL;
	}

	FormatProvider* provider = DisplayManager::getInstance()->getFormatProvider(filename);
	displayer = provider->openDisplayer(this, wxString(filename));

	closeItem->Enable();
	sizer->Add(displayer, 1, wxEXPAND);
	Layout();

	/*if (TXDArchive::isValidFilename(string(filename))) {
		ifstream* stream = new ifstream(filename, ifstream::in | ifstream::binary);
		TXDPanel* panel = new TXDPanel(this, this, txdMenu);

		if (!panel->displayArchive(stream, true)) {
			delete panel;
		} else {
			contentWidget = panel;
			closeItem->Enable();
			sizer->Add(panel, 1, wxEXPAND);
			Layout();
		}
	} else if (
				IMGArchive::isValidIMGFilename(string(filename))
			||  IMGArchive::isValidDIRFilename(string(filename))
	) {
		IMGPanel* panel = new IMGPanel(this, imgMenu, txdMenu);

		if (!panel->displayFile(filename)) {
			delete panel;
		} else {
			contentWidget = panel;
			closeItem->Enable();
			sizer->Add(panel, 1, wxEXPAND);
			Layout();
		}
	} else {
		wxMessageBox(LangGet(Dialog_ErrorUnknownFileFormat), LangGet(Dialog_ErrorTitle),
				wxOK | wxICON_ERROR);
	}*/
}
