/*
 * TXDFormatProvider.cpp
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#include "TXDFormatProvider.h"
#include <txd/TXDArchive.h>
#include "TXDPanel.h"
#include <string>
#include "../guiconfig.h"


bool TXDFormatProvider::canDisplay(const wxString& filename) const
{
	return TXDArchive::isValidFilename(std::string(filename.mb_str()));
}


FileDisplayer* TXDFormatProvider::openDisplayer(wxWindow* parent, DataSource* source)
{
	TXDPanel* panel = new TXDPanel(parent);
	panel->display(source);
	return panel;
}


/*FileDisplayer* TXDFormatProvider::openDisplayer(wxWindow* parent, const wxString& filename)
{
	TXDPanel* panel = new TXDPanel(parent);
	panel->displayFile(filename);
	return panel;
}*/


wxString TXDFormatProvider::getFileWildcard() const
{
	return LangGet("Format_TXD_fileWildcard");
}


wxString TXDFormatProvider::getDescription(const wxString& filename) const
{
	return LangGet("Format_TXD_description");
}
