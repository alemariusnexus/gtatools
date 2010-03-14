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

#ifndef TXDFORMATPROVIDER_H_
#define TXDFORMATPROVIDER_H_

#include "../FormatProvider.h"
#include <wx/wx.h>

class TXDFormatProvider : public FormatProvider {
public:
	virtual bool canDisplay(const wxString& filename) const;
	virtual FileDisplayer* openDisplayer(wxWindow* parent, DataSource* source);
	//virtual FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename);
	virtual wxString getFileWildcard() const;
	virtual wxString getDescription(const wxString& filename) const;
};

#endif /* TXDFORMATPROVIDER_H_ */
