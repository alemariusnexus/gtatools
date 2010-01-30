#include "TXDPanel.h"
#include <IL/il.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <string>


/*void GetFileFormatExtension(char* dest, ILenum format)
{
	switch (format) {
	case IL_BMP:
		strcpy(dest, "bmp");
		break;
	case IL_PNG:
		strcpy(dest, "png");
		break;
	case IL_JPG:
		strcpy(dest, "jpg");
		break;
	case IL_PNM:
		strcpy(dest, "pnm");
		break;
	case IL_PSD:
		strcpy(dest, "psd");
		break;
	case IL_TGA:
		strcpy(dest, "tga");
		break;
	case IL_SGI:
		strcpy(dest, "sgi");
		break;
	}
}*/


/*TXDPanel::TXDPanel(wxWindow* parent, wxWindow* window, wxMenu* menu)
		: TXDPanelPrototype(parent), menu(menu), stream(NULL), autoCloseStream(false)
{
	nameLabel->SetLabel(LangGet(TXDPanel_nameLabel_emptyValue));
	formatDescLabel->SetLabel(LangGet(TXDPanel_formatDescLabel_value));
	bppDescLabel->SetLabel(LangGet(TXDPanel_bppDescLabel_value));
	widthDescLabel->SetLabel(LangGet(TXDPanel_widthDescLabel_value));
	heightDescLabel->SetLabel(LangGet(TXDPanel_heightDescLabel_value));
	alphaTextureDescLabel->SetLabel(LangGet(TXDPanel_alphaTextureDescLabel_value));
	compressionDescLabel->SetLabel(LangGet(TXDPanel_compressionDescLabel_value));
	alphaUsedDescLabel->SetLabel(LangGet(TXDPanel_alphaUsedDescLabel_value));

	//TXDArchive* archive = new TXDArchive(stream);
	//displayArchive(archive);

	wxMenuItem* extractItem = menu->FindItem(wxID_TXD_EXTRACT);
	window->Connect(extractItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(TXDPanel::onExtract), NULL, this);

	extractItem->Enable(true);
}


TXDPanel::~TXDPanel()
{
	if (archive) {
		if (autoCloseStream) {
			//((ifstream*) stream)->close();
			delete stream;
		}

		delete archive;
	}

	wxMenuItem* extractItem = menu->FindItem(wxID_TXD_EXTRACT);
	extractItem->Enable(false);
	GetParent()->Disconnect(extractItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(TXDPanel::onExtract));
}*/


TXDPanel::TXDPanel(wxWindow* parent)
		: TXDPanelPrototype(parent)
{
	nameLabel->SetLabel(LangGet(TXDPanel_nameLabel_emptyValue));
	formatDescLabel->SetLabel(LangGet(TXDPanel_formatDescLabel_value));
	bppDescLabel->SetLabel(LangGet(TXDPanel_bppDescLabel_value));
	widthDescLabel->SetLabel(LangGet(TXDPanel_widthDescLabel_value));
	heightDescLabel->SetLabel(LangGet(TXDPanel_heightDescLabel_value));
	alphaTextureDescLabel->SetLabel(LangGet(TXDPanel_alphaTextureDescLabel_value));
	compressionDescLabel->SetLabel(LangGet(TXDPanel_compressionDescLabel_value));
	alphaUsedDescLabel->SetLabel(LangGet(TXDPanel_alphaUsedDescLabel_value));
	extractButton->SetLabel(LangGet(TXDPanel_extractButton_label));

	extractButton->Enable(false);
}


TXDPanel::~TXDPanel()
{
	close();
}


bool TXDPanel::doDisplay(istream* stream)
{
	this->stream = stream;

	try {
		TXDArchive* archive = new TXDArchive(stream);
		displayArchive(archive);
	} catch (TXDException ex) {
		wxMessageBox(wxT("Error opening file!"), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	return true;
}


void TXDPanel::doClose()
{
	nameLabel->SetLabel(LangGet(TXDPanel_nameLabel_emptyValue));
	formatLabel->SetLabel(wxT("-"));
	bppLabel->SetLabel(wxT("-"));
	widthLabel->SetLabel(wxT("-"));
	heightLabel->SetLabel(wxT("-"));
	alphaTextureLabel->SetLabel(wxT("-"));
	compressionLabel->SetLabel(wxT("-"));
	alphaUsedLabel->SetLabel(wxT("-"));

	delete archive;
}


/*bool TXDPanel::displayArchive(istream* stream, bool autoCloseStream)
{
	if (autoCloseStream  &&  this->stream) {
		delete this->stream;
	}

	this->stream = stream;
	this->autoCloseStream = autoCloseStream;

	try {
		TXDArchive* archive = new TXDArchive(stream);
		displayArchive(archive);
	} catch (TXDException ex) {
		wxMessageBox("Error opening file!", "Error", wxOK | wxICON_ERROR);
		return false;
	}

	return true;
}*/


void TXDPanel::displayArchive(TXDArchive* archive)
{
	this->archive = archive;

	for (int16_t i = 0 ; i < archive->getTextureCount() ; i++) {
		TXDTexture* texture = archive->nextTexture();
		textureList->Append(wxString(texture->getDiffuseName(), wxConvUTF8), texture);
	}

	if (archive->getTextureCount() != 0) {
		textureList->SetSelection(0);

		TXDTexture* firstTex = (TXDTexture*) textureList->GetClientData(0);
		displayTexture(firstTex);
	}
}


void TXDPanel::onTextureSelectionChanged(wxCommandEvent& evt)
{
	wxArrayInt selections;
	int numSel = textureList->GetSelections(selections);

	if (numSel == 1) {
		TXDTexture* texture = (TXDTexture*) textureList->GetClientData(selections[0]);
		displayTexture(texture);
	}
}


void TXDPanel::onExtract(wxCommandEvent& evt)
{
	wxArrayInt selections;
	int numSel = textureList->GetSelections(selections);

	wxString path;
	ImageFileFormat* format = NULL;

	if (numSel == 1) {
		TXDTexture* texture = (TXDTexture*) textureList->GetClientData(selections[0]);

		wxString wildcards;

		ImageFileFormat** supportedFormats = new ImageFileFormat*[GetSupportedImageFormatCount()];
		int numFormats = GetSupportedImageFormats(supportedFormats);

		for (int i = 0 ; i < numFormats ; i++) {
			if (i != 0) {
				wildcards.Append(wxT("|"));
			}

			ImageFileFormat* format = supportedFormats[i];
			wildcards.Append(format->description).Append(wxT(" (*."))
					.Append(wxString(format->extension, wxConvUTF8)).Append(wxT(")|*."))
					.Append(wxString(format->extension, wxConvUTF8)).Append(wxT(")"));

			delete format;
		}

		delete[] supportedFormats;

		wxFileDialog fd(NULL, LangGet(TXDPanel_dlgExtractItem_title),
				wxString(getenv("HOME"), wxConvUTF8),
				wxString(texture->getDiffuseName(), wxConvUTF8) + wxT(".png"), wildcards, wxFD_SAVE);

		if (fd.ShowModal() != wxID_OK) {
			return;
		}

		path = fd.GetPath();
	} else {
		wxDirDialog dd(NULL, LangGet(TXDPanel_dlgExtractItems_title),
				wxString(getenv("HOME"), wxConvUTF8), wxDD_DIR_MUST_EXIST);

		if (dd.ShowModal() != wxID_OK) {
			return;
		}

		path = dd.GetPath();

		wxArrayString choices;

		ImageFileFormat** supportedFormats = new ImageFileFormat*[GetSupportedImageFormatCount()];
		int numFormats = GetSupportedImageFormats(supportedFormats);

		for (int i = 0 ; i < numFormats ; i++) {
			format = supportedFormats[i];
			choices.Add(format->description);
		}

		int idx = wxGetSingleChoiceIndex(LangGet(TXDPanel_dlgExtractFormat_title), wxT("Test Test"),
				choices);

		if (idx == -1) {
			return;
		}

		format = supportedFormats[idx];

		for (int i = 0 ; i < numFormats ; i++) {
			if (supportedFormats[i] != format) {
				delete supportedFormats[i];
			}
		}

		delete[] supportedFormats;
	}

	for (int i = 0 ; i < numSel ; i++) {
		TXDTexture* texture = (TXDTexture*) textureList->GetClientData(selections[i]);

		ilBindImage(1);

		archive->gotoTexture(texture);
		uint8_t* rawData = archive->readTextureData(texture);
		int16_t width = texture->getWidth();
		uint16_t height = texture->getHeight();
		uint8_t* data;

		if (texture->hasAlphaChannel()) {
			data = new uint8_t[width * height * 4];
			texture->convert(data, rawData);
			delete[] rawData;

			ilTexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, data);
		} else {
			data = new uint8_t[width * height * 3];
			texture->convert(data, rawData, MIRROR_HORIZONTAL, 3, 0, 1, 2, -1);
			delete[] rawData;

			ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, data);
		}

		if (numSel == 1) {
			ilSaveImage(path.c_str());
		} else {
			wxString file = wxString(path, wxConvUTF8) + wxT("/")
					+ wxString(texture->getDiffuseName(), wxConvUTF8) + wxT(".")
					+ wxString(format->extension, wxConvUTF8);
			ilSave(format->format, file.c_str());
		}

		ilDeleteImage(1);
		delete[] data;
	}

	if (numSel != 1) {
		delete format;
	}
}


void TXDPanel::displayTexture(TXDTexture* texture)
{
	char buffer[16];

	nameLabel->SetLabel(wxString(texture->getDiffuseName(), wxConvUTF8));

	TxdGetRasterFormatName(buffer, texture->getRasterFormat());
	formatLabel->SetLabel(wxString(buffer, wxConvUTF8));

	//sprintf(buffer, "%d", texture->getBytesPerPixel());
	bppLabel->SetLabel(wxString::Format(wxT("%d"), texture->getBytesPerPixel()));

	//sprintf(buffer, "%d", texture->getWidth());
	widthLabel->SetLabel(wxString::Format(wxT("%d"), texture->getWidth()));

	//sprintf(buffer, "%d", texture->getHeight());
	heightLabel->SetLabel(wxString::Format(wxT("%d"), texture->getHeight()));

	alphaTextureLabel->SetLabel(wxString(texture->getAlphaName(), wxConvUTF8));

	alphaUsedLabel->SetLabel(texture->hasAlphaChannel() ? LangGet(Misc_Yes) : LangGet(Misc_No));

	switch (texture->getCompression()) {
	case DXT1:
		compressionLabel->SetLabel(LangGet(TXDPanel_compression_DXT1));
		break;
	case DXT3:
		compressionLabel->SetLabel(LangGet(TXDPanel_compression_DXT3));
		break;
	case NONE:
		compressionLabel->SetLabel(LangGet(TXDPanel_compression_None));
		break;
	}

	archive->gotoTexture(texture);
	uint8_t* data = archive->readTextureData(texture);
	image->displayTexture(texture, data);
	delete[] data;

	extractButton->Enable();
}


bool TXDPanel::canDisplay(const wxString& filename)
{
	return TXDArchive::isValidFilename(string(filename.mb_str()));
}
