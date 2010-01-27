#include "TXDPanel.h"
#include <IL/il.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/choicdlg.h>


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


TXDPanel::TXDPanel(wxWindow* parent, wxWindow* window, istream* stream, wxMenu* menu,
		bool autoCloseStream)
		: TXDPanelPrototype(parent), menu(menu), stream(stream), autoCloseStream(autoCloseStream)
{
	nameLabel->SetLabel(LangGet(TXDPanel_nameLabel_emptyValue));
	formatDescLabel->SetLabel(LangGet(TXDPanel_formatDescLabel_value));
	bppDescLabel->SetLabel(LangGet(TXDPanel_bppDescLabel_value));
	widthDescLabel->SetLabel(LangGet(TXDPanel_widthDescLabel_value));
	heightDescLabel->SetLabel(LangGet(TXDPanel_heightDescLabel_value));
	alphaTextureDescLabel->SetLabel(LangGet(TXDPanel_alphaTextureDescLabel_value));
	compressionDescLabel->SetLabel(LangGet(TXDPanel_compressionDescLabel_value));
	alphaUsedDescLabel->SetLabel(LangGet(TXDPanel_alphaUsedDescLabel_value));

	TXDArchive* archive = new TXDArchive(stream);
	displayArchive(archive);

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
}


void TXDPanel::displayArchive(TXDArchive* archive)
{
	this->archive = archive;

	for (int16_t i = 0 ; i < archive->getTextureCount() ; i++) {
		TXDTexture* texture = archive->nextTexture();
		textureList->Append(texture->getDiffuseName(), texture);
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
				wildcards.Append("|");
			}

			ImageFileFormat* format = supportedFormats[i];
			wildcards.Append(format->description).Append(" (*.").Append(format->extension)
					.Append(")|*.").Append(format->extension).Append(")");

			delete format;
		}

		delete[] supportedFormats;

		wxFileDialog fd(NULL, LangGet(TXDPanel_dlgExtractItem_title), getenv("HOME"),
				wxString(texture->getDiffuseName()) + ".png",
				wildcards,
				wxFD_SAVE);

		if (fd.ShowModal() != wxID_OK) {
			return;
		}

		path = fd.GetPath();
	} else {
		wxDirDialog dd(NULL, LangGet(TXDPanel_dlgExtractItem_title), getenv("HOME"), wxDD_DIR_MUST_EXIST);

		if (dd.ShowModal() != wxID_OK) {
			return;
		}

		path = dd.GetPath();

		wxArrayString choices;

		ImageFileFormat** supportedFormats = new ImageFileFormat*[GetSupportedImageFormatCount()];
		int numFormats = GetSupportedImageFormats(supportedFormats);

		//for (unsigned int i = 0 ; i < sizeof(SupportedImageFormats)/sizeof(ImageFileFormat) ; i++) {
		for (int i = 0 ; i < numFormats ; i++) {
			format = supportedFormats[i];
			choices.Add(format->description);
		}

		int idx = wxGetSingleChoiceIndex(LangGet(TXDPanel_dlgExtractFormat_title), "Test Test", choices);

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
			ilSaveImage(path);
		} else {
			wxString file = wxString(path) + wxString("/") + wxString(texture->getDiffuseName())
					+ wxString(".") + format->extension;
			ilSave(format->format, file);
			delete format;
		}

		ilDeleteImage(1);
		delete[] data;
	}
}


void TXDPanel::displayTexture(TXDTexture* texture)
{
	char buffer[16];

	nameLabel->SetLabel(wxString(texture->getDiffuseName()));

	TxdGetRasterFormatName(buffer, texture->getRasterFormat());
	formatLabel->SetLabel(wxString(buffer));

	sprintf(buffer, "%d", texture->getBytesPerPixel());
	bppLabel->SetLabel(wxString(buffer));

	sprintf(buffer, "%d", texture->getWidth());
	widthLabel->SetLabel(wxString(buffer));

	sprintf(buffer, "%d", texture->getHeight());
	heightLabel->SetLabel(wxString(buffer));

	alphaTextureLabel->SetLabel(wxString(texture->getAlphaName()));

	alphaUsedLabel->SetLabel(wxString(texture->hasAlphaChannel() ? LangGet(Misc_Yes) : LangGet(Misc_No)));

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
}
