/*
 * DisplayManager.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef DISPLAYMANAGER_H_
#define DISPLAYMANAGER_H_

#include "FileDisplayer.h"
#include "FormatProvider.h"
#include <wx/wx.h>
#include <istream>

using std::istream;

class DisplayManager {
public:
	static DisplayManager* getInstance();

public:
	~DisplayManager();
	//FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename);
	//FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename, istream* stream);
	FormatProvider* getFormatProvider(const wxString& filename);
	void closeDisplayer(FileDisplayer* displayer);
	FormatProvider** getFormatProviders();
	short getFormatProviderCount() { return numProviders; }

private:
	DisplayManager();

private:
	FormatProvider** providers;
	short numProviders;
};

#endif /* DISPLAYMANAGER_H_ */
