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

#include "IMGPanel.h"
#include <gtaformats/img/IMGException.h>
#include <cstdio>
#include "../guiconfig.h"
#include <wx/msgdlg.h>
#include <string>
#include "../DisplayManager.h"
#include "../FormatProvider.h"
#include "../FileDataSource.h"
#include <iostream>
#include <wx/filedlg.h>
#include "../MainFrame.h"
#include <gtaformats/util/stream/InputStream.h>

using std::string;
using std::ofstream;
using std::cout;
using std::endl;



IMGPanel::IMGPanel(wxWindow* parent)
		: IMGPanelPrototype(parent), entryDisplayer(NULL)
{
	imgVersionDescLabel->SetLabel(LangGet("IMGPanel_imgVersionDescLabel_value"));
	imgNumEntriesDescLabel->SetLabel(LangGet("IMGPanel_imgNumEntriesDescLabel_value"));
	typeDescLabel->SetLabel(LangGet("IMGPanel_typeDescLabel_value"));
	offsetDescLabel->SetLabel(LangGet("IMGPanel_offsetDescLabel_value"));
	sizeDescLabel->SetLabel(LangGet("IMGPanel_sizeDescLabel_value"));

	extractButton->SetLabel(LangGet("IMGPanel_extractButton_label"));

	entryNotebook->SetPageText(0, LangGet("IMGPanel_entryInfoPanel_label"));

	notebook->SetPageText(0, LangGet("IMGPanel_infoPanel_label"));
	notebook->SetPageText(1, LangGet("IMGPanel_entriesPanel_label"));
}


IMGPanel::~IMGPanel()
{
	close();
}


bool IMGPanel::doDisplay(DataSource* source)
{
	InputStream* stream = source->getStream();

	IMGArchive::IMGVersion version = IMGArchive::guessIMGVersion(stream);

	if (version == IMGArchive::VER1) {
		FileDataSource* fileSource = (FileDataSource*) source;
		const wxString& fname = fileSource->getFilename();
		InputStream* dirStream = NULL;
		InputStream* imgStream = NULL;
		wxString prefix = fname.substr(0, fname.find_last_of('.'));

		wxString imgFile;
		wxString dirFile;

		if (IMGArchive::isValidDIRFilename(string(fname.mb_str()))) {
			dirStream = stream;
			imgFile = prefix.Append(wxT(".img"));
			dirFile = fname;
			imgStream = new FileInputStream(imgFile.mb_str(), STREAM_BINARY);
		} else if (IMGArchive::isValidIMGFilename(string(fname.mb_str()))) {
			imgStream = stream;
			imgFile = fname;
			dirFile = prefix.Append(wxT(".dir"));
			dirStream = new FileInputStream(dirFile.mb_str(), STREAM_BINARY);
		}

		cout << "Opening VER1 IMG file " << imgFile.mb_str() << " (" << dirFile.mb_str() << ")..." << endl;

		archive = new IMGArchive(dirStream, imgStream, true);

		source->setStream(imgStream);
		delete dirStream;
	} else if (version == IMGArchive::VER2) {
		cout << "Opening a VER2 IMG file..." << endl;
		archive = new IMGArchive(stream, true);
	}

	displayFiltered(wxString(wxT("")));

	wxString fileName = source->getName();
	imgFileLabel->SetLabel(LangGet("IMGPanel_imgFileLabel_label", fileName.c_str()));

	if (version == IMGArchive::VER1) {
		imgVersionLabel->SetLabel(LangGet("Format_IMG_VER1"));
	} else if (version == IMGArchive::VER2) {
		imgVersionLabel->SetLabel(LangGet("Format_IMG_VER2"));
	}

	imgNumEntriesLabel->SetLabel(wxString::Format(wxT("%d"), archive->getEntryCount()));

	return true;
}


void IMGPanel::displayFiltered(const wxString& filter)
{
	wxString lFilter = filter.Lower();

	fileList->Clear();
	IMGEntry** entries = archive->getEntries();

	for (int32_t i = 0 ; i < archive->getEntryCount() ; i++) {
		wxString name(entries[i]->name, wxConvUTF8);

		if (name.Lower().Contains(lFilter)) {
			fileList->Append(name, entries[i]);
		}
	}
}


void IMGPanel::doClose()
{
	delete archive;
}


void IMGPanel::onSelectionChanged(wxCommandEvent& evt)
{
	int selBefore = entryNotebook->GetSelection();

	if (entryDisplayer != NULL) {
		entryNotebook->RemovePage(1);
		DisplayManager::getInstance()->closeDisplayer(entryDisplayer);
		entryDisplayer = NULL;
		entrySizer->Remove(entryContentSizer);
	}

	wxArrayInt selections;
	int numSel = fileList->GetSelections(selections);

	if (numSel == 1) {
		IMGEntry* entry = (IMGEntry*) fileList->GetClientData(selections[0]);

		fileLabel->SetLabel(LangGet("IMGPanel_fileLabel_label",
				wxString(entry->name, wxConvUTF8).c_str()));

		offsetLabel->SetLabel(wxString::Format(wxT("%d"), entry->offset*IMG_BLOCK_SIZE));

		sizeLabel->SetLabel(wxString::Format(wxT("%d"), entry->size*IMG_BLOCK_SIZE));

		InputStream* stream = archive->gotoEntry(entry);
		DataSource* source = new DataSource(stream, wxString(entry->name, wxConvUTF8), false);
		FormatProvider* provider = DisplayManager::getInstance()
				->getFormatProvider(wxString(entry->name, wxConvUTF8));

		if (provider != NULL) {
			typeLabel->SetLabel(provider->getDescription(wxString(entry->name, wxConvUTF8)));

			if (provider->isImplemented()) {
				entryDisplayer = provider->openDisplayer(entryNotebook, source);
				entryNotebook->AddPage(entryDisplayer, LangGet("IMGPanel_entryContentPanel_label"));

				if (selBefore != -1) {
					entryNotebook->SetSelection(selBefore);
				}

				entryNotebook->Refresh();
			}
		} else {
			typeLabel->SetLabel(LangGet("Format_Unknown_description"));
		}
	}
}


void IMGPanel::onExtract(wxCommandEvent& evt)
{
	wxArrayInt selections;
	int numSel = fileList->GetSelections(selections);

	if (numSel == 1) {
		IMGEntry* entry = (IMGEntry*) fileList->GetClientData(selections[0]);

		wxFileDialog fd(this, LangGet("IMGPanel_dlgExtractItem_title"),
				wxString(getenv("HOME"), wxConvUTF8),
				wxString(entry->name, wxConvUTF8).c_str(), wxT("*.*"), wxFD_SAVE);

		if (fd.ShowModal() == wxID_OK) {
			wxString path = fd.GetPath();

			ofstream out(path.mb_str(), ofstream::out | ofstream::binary);

			InputStream* in = archive->gotoEntry(entry);

			char buffer[IMG_BLOCK_SIZE];

			for (int32_t i = 0 ; i < entry->size ; i++) {
				in->read(buffer, IMG_BLOCK_SIZE);
				out.write(buffer, sizeof(buffer));
			}

			out.close();
		}
	} else {
		wxDirDialog fd(this, LangGet("IMGPanel_dlgExtractItems_title"),
				wxString(getenv("HOME"), wxConvUTF8), wxDD_DIR_MUST_EXIST);

		if (fd.ShowModal() == wxID_OK) {
			wxString path = fd.GetPath();

			for (int i = 0 ; i < numSel ; i++) {
				IMGEntry* entry = (IMGEntry*) fileList->GetClientData(selections[i]);

				wxString fpath = path;
				fpath << wxT("/") << wxString(entry->name, wxConvUTF8);

				ofstream out(fpath.mb_str(), ofstream::out | ofstream::binary);

				InputStream* in = archive->gotoEntry(entry);

				char buffer[IMG_BLOCK_SIZE];

				for (int32_t j = 0 ; j < entry->size ; j++) {
					in->read(buffer, IMG_BLOCK_SIZE);
					out.write(buffer, sizeof(buffer));
				}

				out.close();
			}
		}
	}
}


void IMGPanel::onApplyFilter(wxCommandEvent& evt)
{
	displayFiltered(filterField->GetValue());
}


bool IMGPanel::canDisplay(const wxString& filename)
{
	return IMGArchive::isValidIMGFilename(string(filename.mb_str()));
}
