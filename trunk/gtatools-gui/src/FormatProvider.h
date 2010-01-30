/*
 * FormatProvider.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef FORMATPROVIDER_H_
#define FORMATPROVIDER_H_

#include "FileDisplayer.h"

class FormatProvider {
public:
	virtual bool canDisplay(const wxString& filename) const = 0;
	virtual FileDisplayer* openDisplayer(wxWindow* parent, istream* stream) = 0;
	virtual FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename) = 0;
	virtual wxString getFileWildcard() const = 0;
	virtual wxString getDescription(const wxString& filename) const = 0;
};

#endif /* FORMATPROVIDER_H_ */
