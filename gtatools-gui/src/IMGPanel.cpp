#include "IMGPanel.h"
#include "TXDPanel.h"
#include <IMGException.h>
#include <TXDArchive.h>
#include <cstdio>
#include <fstream>
#include "lang/lang.h"
#include <wx/msgdlg.h>
#include <string>
#include "DisplayManager.h"
#include "FormatProvider.h"

using std::istream;
using std::ofstream;

/*IMGPanel::IMGPanel(wxWindow* parent, wxMenu* menu, wxMenu* txdMenu)
		: IMGPanelPrototype(parent), menu(menu), txdMenu(txdMenu), pluginWindow(NULL)
{
	typeDescLabel->SetLabel(LangGet(IMGPanel_typeDescLabel_value));
	offsetDescLabel->SetLabel(LangGet(IMGPanel_offsetDescLabel_value));
	sizeDescLabel->SetLabel(LangGet(IMGPanel_sizeDescLabel_value));
}


IMGPanel::~IMGPanel()
{
	//stream->close();
	//delete stream;
	delete archive;
}


bool IMGPanel::displayFile(const char* filename)
{
	try {
		archive = new IMGArchive(filename);
	} catch (IMGException ex) {
		char error[256];
		LangGetFormatted(Dialog_ErrorOpeningIMG, error, ex.what());
		wxMessageBox(error, "Error", wxICON_ERROR | wxOK);
		return false;
	}

	IMGEntry** entries = archive->getEntries();

	for (int32_t i = 0 ; i < archive->getEntryCount() ; i++) {
		fileList->Append(entries[i]->name, entries[i]);
	}

	return true;
}*/

IMGPanel::IMGPanel(wxWindow* parent)
		: IMGPanelPrototype(parent), entryDisplayer(NULL)
{
	typeDescLabel->SetLabel(LangGet(IMGPanel_typeDescLabel_value));
	offsetDescLabel->SetLabel(LangGet(IMGPanel_offsetDescLabel_value));
	sizeDescLabel->SetLabel(LangGet(IMGPanel_sizeDescLabel_value));
}


IMGPanel::~IMGPanel()
{
	close();
}


bool IMGPanel::doDisplay(istream* stream)
{
	try {
		archive = new IMGArchive(stream);
	} catch (IMGException ex) {
		//char error[256];
		//LangGetFormatted(Dialog_ErrorOpeningIMG, error, ex.what());
		wxString error = LangGetFormatted(Dialog_ErrorOpeningIMG, ex.what());
		wxMessageBox(error, wxT("Error"), wxICON_ERROR | wxOK);
		return false;
	}

	IMGEntry** entries = archive->getEntries();

	for (int32_t i = 0 ; i < archive->getEntryCount() ; i++) {
		fileList->Append(wxString(entries[i]->name, wxConvUTF8), entries[i]);
	}

	return true;
}


void IMGPanel::doClose()
{
	delete archive;
}


void IMGPanel::onSelectionChanged(wxCommandEvent& evt)
{
	if (entryDisplayer != NULL) {
		DisplayManager::getInstance()->closeDisplayer(entryDisplayer);
		entryDisplayer = NULL;
	}

	wxArrayInt selections;
	int numSel = fileList->GetSelections(selections);

	if (numSel == 1) {
		IMGEntry* entry = (IMGEntry*) fileList->GetClientData(selections[0]);

		//char buffer[16];

		fileLabel->SetLabel(wxString(entry->name, wxConvUTF8));

		//sprintf(buffer, "%d", entry->offset*IMG_BLOCK_SIZE);
		//offsetLabel->SetLabel(wxString(buffer));
		offsetLabel->SetLabel(wxString::Format(wxT("%d"), entry->offset*IMG_BLOCK_SIZE));

		//sprintf(buffer, "%d", entry->size*IMG_BLOCK_SIZE);
		//sizeLabel->SetLabel(wxString(buffer));
		sizeLabel->SetLabel(wxString::Format(wxT("%d"), entry->size*IMG_BLOCK_SIZE));

		istream* stream = archive->gotoEntry(entry);
		FormatProvider* provider = DisplayManager::getInstance()
				->getFormatProvider(wxString(entry->name, wxConvUTF8));

		if (provider != NULL) {
			entryDisplayer = provider->openDisplayer(infoPanel, stream);
			infoSizer->Add(entryDisplayer, 1, wxEXPAND);
			infoPanel->Layout();
			typeLabel->SetLabel(provider->getDescription(wxString(entry->name, wxConvUTF8)));
		} else {
			typeLabel->SetLabel(LangGet(Format_Unknown_description));
		}

		/*if (TXDArchive::isValidFilename(entry->name)) {
			istream* stream = archive->gotoEntry(entry);
			TXDPanel* panel = new TXDPanel(infoPanel, GetParent(), txdMenu);
			panel->displayArchive(stream, false);
			pluginWindow = panel;
			infoSizer->Add(panel, 1, wxEXPAND);
			infoPanel->Layout();

			typeLabel->SetLabel(wxString(LangGet(Format_TXD_description)));
		} else if (wxString(entry->name).EndsWith(".dff")) {
			typeLabel->SetLabel(wxString(LangGet(Format_DFF_description)));
		} else {
			typeLabel->SetLabel(wxString(LangGet(Format_Unknown_description)));
		}*/
	}
}


bool IMGPanel::canDisplay(const wxString& filename)
{
	return IMGArchive::isValidIMGFilename(string(filename.mb_str()));
}
