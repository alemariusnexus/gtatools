/*
 * IMGFormatProvider.cpp
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#include "IMGFormatProvider.h"
#include <gtaformats/IMGArchive.h>
#include <string>
#include "IMGPanel.h"
#include <wx/wx.h>
#include "lang/lang.h"


bool IMGFormatProvider::canDisplay(const wxString& filename) const
{
	return IMGArchive::isValidIMGFilename(std::string(filename.c_str()));
}


FileDisplayer* IMGFormatProvider::openDisplayer(wxWindow* parent, istream* stream)
{
	IMGPanel* panel = new IMGPanel(parent);
	panel->display(stream);
	return panel;
}


FileDisplayer* IMGFormatProvider::openDisplayer(wxWindow* parent, const wxString& filename)
{
	IMGPanel* panel = new IMGPanel(parent);
	panel->displayFile(filename);
	return panel;
}


wxString IMGFormatProvider::getFileWildcard() const
{
	return LangGet(Format_IMG_fileWildcard);
}


wxString IMGFormatProvider::getDescription(const wxString& filename) const
{
	return LangGet(Format_IMG_description);
}
