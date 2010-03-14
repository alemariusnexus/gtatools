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

#include "TXDFormatProvider.h"
#include <gtaformats/txd/TXDArchive.h>
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
