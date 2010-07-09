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

#include "DisplayManager.h"
#include "FormatProvider.h"
#include <fstream>
#include "img/IMGFormatProvider.h"
#include "txd/TXDFormatProvider.h"
#include "dff/DFFFormatProvider.h"

using std::ifstream;


DisplayManager* DisplayManager::getInstance()
{
	static DisplayManager* instance = new DisplayManager();
	return instance;
}


DisplayManager::DisplayManager()
{
	//providers = new FormatProvider*[];

	numProviders = 3;
	providers = new FormatProvider*[numProviders];
	providers[0] = new IMGFormatProvider;
	providers[1] = new TXDFormatProvider;
	providers[2] = new DFFFormatProvider;
}


DisplayManager::~DisplayManager()
{
	for (int i = 0 ; i < numProviders ; i++) {
		if (providers[i]) {
			delete providers[i];
		}
	}
}


/*FileDisplayer* DisplayManager::openDisplayer(wxWindow* parent, const wxString& filename)
{
	for (int i = 0 ; i < numProviders ; i++) {
		if (providers[i]->canDisplay(filename)) {
			FileDisplayer* displayer = providers[i]->openDisplayer(parent, filename);
			return displayer;
		}
	}

	return NULL;
}


FileDisplayer* DisplayManager::openDisplayer(wxWindow* parent, const wxString& filename, istream* stream)
{
	for (int i = 0 ; i < numProviders ; i++) {
		if (providers[i]->canDisplay(filename)) {
			FileDisplayer* displayer = providers[i]->openDisplayer(parent, stream);
			return displayer;
		}
	}

	return NULL;
}*/


FormatProvider* DisplayManager::getFormatProvider(const wxString& filename)
{
	for (int i = 0 ; i < numProviders ; i++) {
		if (providers[i]->canDisplay(filename)) {
			return providers[i];
		}
	}

	return NULL;
}


void DisplayManager::closeDisplayer(FileDisplayer* displayer)
{
	displayer->close();
	delete displayer;
}


FormatProvider** DisplayManager::getFormatProviders()
{
	return providers;
}
