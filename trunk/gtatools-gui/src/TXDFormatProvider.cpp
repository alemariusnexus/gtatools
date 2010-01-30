/*
 * TXDFormatProvider.cpp
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#include "TXDFormatProvider.h"
#include <gtaformats/TXDArchive.h>
#include "TXDPanel.h"
#include <string>
#include "lang/lang.h"


bool TXDFormatProvider::canDisplay(const wxString& filename) const
{
	return TXDArchive::isValidFilename(std::string(filename.c_str()));
}


FileDisplayer* TXDFormatProvider::openDisplayer(wxWindow* parent, istream* stream)
{
	TXDPanel* panel = new TXDPanel(parent);
	panel->display(stream);
	return panel;
}


FileDisplayer* TXDFormatProvider::openDisplayer(wxWindow* parent, const wxString& filename)
{
	TXDPanel* panel = new TXDPanel(parent);
	panel->displayFile(filename);
	return panel;
}


wxString TXDFormatProvider::getFileWildcard() const
{
	return LangGet(Format_TXD_fileWildcard);
}


wxString TXDFormatProvider::getDescription(const wxString& filename) const
{
	return LangGet(Format_TXD_description);
}
