#include "IMGPanel.h"
#include "TXDPanel.h"
#include <gtaformats/TXDArchive.h>
#include <cstdio>
#include <fstream>
#include "lang/lang.h"

using std::istream;
using std::ofstream;

IMGPanel::IMGPanel(wxWindow* parent, const char* filename, wxMenu* menu, wxMenu* txdMenu)
		: IMGPanelPrototype(parent), menu(menu), txdMenu(txdMenu), pluginWindow(NULL)
{
	typeDescLabel->SetLabel(LangGet(IMGPanel_typeDescLabel_value));
	offsetDescLabel->SetLabel(LangGet(IMGPanel_offsetDescLabel_value));
	sizeDescLabel->SetLabel(LangGet(IMGPanel_sizeDescLabel_value));

	stream = new ifstream(filename, ifstream::in | ifstream::binary);
	archive = new IMGArchive(stream);

	IMGEntry** entries = archive->getEntries();

	for (int32_t i = 0 ; i < archive->getEntryCount() ; i++) {
		fileList->Append(entries[i]->name, entries[i]);
	}
}


IMGPanel::~IMGPanel()
{
	stream->close();
	delete stream;
	delete archive;
}


void IMGPanel::onSelectionChanged(wxCommandEvent& evt)
{
	if (pluginWindow) {
		delete pluginWindow;
		pluginWindow = NULL;
	}

	wxArrayInt selections;
	int numSel = fileList->GetSelections(selections);

	if (numSel == 1) {
		IMGEntry* entry = (IMGEntry*) fileList->GetClientData(selections[0]);

		char buffer[16];

		fileLabel->SetLabel(wxString(entry->name));

		sprintf(buffer, "%d", entry->offset*IMG_BLOCK_SIZE);
		offsetLabel->SetLabel(wxString(buffer));

		sprintf(buffer, "%d", entry->size*IMG_BLOCK_SIZE);
		sizeLabel->SetLabel(wxString(buffer));

		if (TXDArchive::isValidFilename(entry->name)) {
			istream* stream = archive->gotoEntry(entry);
			TXDPanel* panel = new TXDPanel(infoPanel, GetParent(), stream,
					txdMenu, false);
			pluginWindow = panel;
			infoSizer->Add(panel, 1, wxEXPAND);
			infoPanel->Layout();

			typeLabel->SetLabel(wxString(LangGet(Format_TXD_description)));
		} else {
			typeLabel->SetLabel(wxString(LangGet(Format_Unknown_description)));
		}
	}
}
