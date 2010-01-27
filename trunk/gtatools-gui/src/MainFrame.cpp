#include "MainFrame.h"
#include "TXDPanel.h"
#include "IMGPanel.h"

#include <gtaformats/TXDArchive.h>
#include <string>
#include <fstream>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include "lang/lang.h"

using std::string;
using std::ifstream;


MainFrame::MainFrame( wxWindow* parent )
		: MainFramePrototype( parent )
{
	SetTitle(LangGet(MainFrame_title));

	menuBar->SetMenuLabel(0, LangGet(MainFrame_fileMenu_label));
	menuBar->SetMenuLabel(1, LangGet(MainFrame_txdMenu_label));
	menuBar->SetMenuLabel(2, LangGet(MainFrame_imgMenu_label));
	//fileMenu->SetTitle(LangGet(MainFrame_fileMenu_label));
	//imgMenu->SetTitle(LangGet(MainFrame_imgMenu_label));
	//txdMenu->SetTitle(LangGet(MainFrame_txdMenu_label));
	openItem->SetItemLabel(LangGet(MainFrame_openItem_label));
	closeItem->SetItemLabel(LangGet(MainFrame_closeItem_label));
	txdExtractItem->SetItemLabel(LangGet(MainFrame_txdExtractItem_label));
}

void MainFrame::onOpen( wxCommandEvent& event )
{
	//wxFileDialog fd(NULL);
	wxString path = wxFileSelector(LangGet(MainFrame_dlgOpenFile_title), getenv("HOME"), "", "",
			LangGet(MainFrame_dlgOpenFile_wildcards));

	//sizer = new wxBoxSizer(wxVERTICAL);

	if (!path.empty()) {
		displayFile(path);
	}
}


void MainFrame::onClose(wxCommandEvent& evt)
{
	closeItem->Enable(false);
	delete contentWidget;
	contentWidget = NULL;
}


void MainFrame::displayFile(const char* filename)
{
	if (contentWidget) {
		delete contentWidget;
		contentWidget = NULL;
	}

	if (TXDArchive::isValidFilename(string(filename))) {
		ifstream* stream = new ifstream(filename, ifstream::in | ifstream::binary);
		TXDPanel* panel = new TXDPanel(this, this, stream, txdMenu, true);
		contentWidget = panel;
		closeItem->Enable();
		sizer->Add(panel, 1, wxEXPAND);
		Layout();
	} else if (IMGArchive::isValidFilename(string(filename))) {
		IMGPanel* panel = new IMGPanel(this, filename, imgMenu, txdMenu);
		contentWidget = panel;
		closeItem->Enable();
		sizer->Add(panel, 1, wxEXPAND);
		Layout();
	} else {
		wxMessageBox("The file has an unknown format!", "Unknown Format", wxOK | wxICON_ERROR);
	}
}
