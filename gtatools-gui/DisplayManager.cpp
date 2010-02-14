/*
 * DisplayManager.cpp
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#include "DisplayManager.h"
#include "FormatProvider.h"
#include "TXDPanel.h"
#include "IMGPanel.h"
#include <fstream>
#include "IMGFormatProvider.h"
#include "TXDFormatProvider.h"

using std::ifstream;


DisplayManager* DisplayManager::getInstance()
{
	static DisplayManager* instance = new DisplayManager();
	return instance;
}


DisplayManager::DisplayManager()
{
	//providers = new FormatProvider*[];

	numProviders = 2;
	providers = new FormatProvider*[numProviders];
	providers[0] = new IMGFormatProvider;
	providers[1] = new TXDFormatProvider;
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
