#include "IMGPanel.h"
#include "TXDPanel.h"
#include <IMGException.h>
#include <cstdio>
#include <fstream>
#include "lang/lang.h"
#include <wx/msgdlg.h>
#include <string>
#include "DisplayManager.h"
#include "FormatProvider.h"
#include "FileDataSource.h"
#include <iostream>

using std::istream;
using std::ofstream;
using std::cout;
using std::endl;

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
	imgVersionDescLabel->SetLabel(LangGet(IMGPanel_imgVersionDescLabel_value));
	imgNumEntriesDescLabel->SetLabel(LangGet(IMGPanel_imgNumEntriesDescLabel_value));
	typeDescLabel->SetLabel(LangGet(IMGPanel_typeDescLabel_value));
	offsetDescLabel->SetLabel(LangGet(IMGPanel_offsetDescLabel_value));
	sizeDescLabel->SetLabel(LangGet(IMGPanel_sizeDescLabel_value));
}


IMGPanel::~IMGPanel()
{
	close();
}


bool IMGPanel::doDisplay(DataSource* source)
{
	istream* stream = source->getStream();

	IMGArchive::IMGVersion version = IMGArchive::guessIMGVersion(stream);

	if (version == IMGArchive::VER1) {
		FileDataSource* fileSource = (FileDataSource*) source;
		const wxString& fname = fileSource->getFilename();
		istream* dirStream = NULL;
		istream* imgStream = NULL;
		wxString prefix = fname.substr(0, fname.find_last_of('.'));

		wxString imgFile;
		wxString dirFile;

		cout << "It is " << fname.mb_str() << " (" << fileSource->getName().mb_str() << ")" << endl;

		if (IMGArchive::isValidDIRFilename(string(fname.mb_str()))) {
			dirStream = stream;
			imgFile = prefix.Append(wxT(".img"));
			dirFile = fname;
			imgStream = new ifstream(imgFile.mb_str(), ifstream::in | ifstream::binary);
			cout << "1" << endl;
		} else if (IMGArchive::isValidIMGFilename(string(fname.mb_str()))) {
			imgStream = stream;
			imgFile = fname;
			dirFile = prefix.Append(wxT(".dir"));
			dirStream = new ifstream(dirFile.mb_str(), ifstream::in | ifstream::binary);
			cout << "2" << endl;
		}

		cout << "Opening VER1 IMG file " << imgFile.mb_str() << " (" << dirFile.mb_str() << ")..." << endl;

		archive = new IMGArchive(dirStream, imgStream, true);

		source->setStream(imgStream);
		delete dirStream;
	} else if (version == IMGArchive::VER2) {
		cout << "Opening a VER2 IMG file..." << endl;
		archive = new IMGArchive(stream, true);
	}

	cout << "  -> " << archive->getEntryCount() << " entries found" << endl;

	/*try {
		archive = new IMGArchive(stream);
	} catch (IMGException ex) {
		wxString error = LangGetFormatted(Dialog_ErrorOpeningIMG, ex.what());
		wxMessageBox(error, wxT("Error"), wxICON_ERROR | wxOK);
		return false;
	}*/

	IMGEntry** entries = archive->getEntries();

	for (int32_t i = 0 ; i < archive->getEntryCount() ; i++) {
		fileList->Append(wxString(entries[i]->name, wxConvUTF8), entries[i]);
	}

	imgFileLabel->SetLabel(source->getName());

	if (version == IMGArchive::VER1) {
		imgVersionLabel->SetLabel(LangGet(Format_IMG_VER1));
	} else if (version == IMGArchive::VER2) {
		imgVersionLabel->SetLabel(LangGet(Format_IMG_VER2));
	}

	imgNumEntriesLabel->SetLabel(wxString::Format(wxT("%d"), archive->getEntryCount()));

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

		fileLabel->SetLabel(wxString(entry->name, wxConvUTF8));

		offsetLabel->SetLabel(wxString::Format(wxT("%d"), entry->offset*IMG_BLOCK_SIZE));

		sizeLabel->SetLabel(wxString::Format(wxT("%d"), entry->size*IMG_BLOCK_SIZE));

		istream* stream = archive->gotoEntry(entry);
		DataSource* source = new DataSource(stream, wxString(entry->name, wxConvUTF8), false);
		FormatProvider* provider = DisplayManager::getInstance()
				->getFormatProvider(wxString(entry->name, wxConvUTF8));

		if (provider != NULL) {
			entryDisplayer = provider->openDisplayer(infoPanel, source);
			infoSizer->Add(entryDisplayer, 1, wxEXPAND);
			infoPanel->Layout();
			typeLabel->SetLabel(provider->getDescription(wxString(entry->name, wxConvUTF8)));
		} else {
			typeLabel->SetLabel(LangGet(Format_Unknown_description));
		}
	}
}


bool IMGPanel::canDisplay(const wxString& filename)
{
	return IMGArchive::isValidIMGFilename(string(filename.mb_str()));
}
