/*
 * IMGFormatProvider.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef IMGFORMATPROVIDER_H_
#define IMGFORMATPROVIDER_H_

#include "FormatProvider.h"

class IMGFormatProvider: public FormatProvider {
public:
	virtual bool canDisplay(const wxString& filename) const;
	virtual FileDisplayer* openDisplayer(wxWindow* parent, istream* stream);
	virtual FileDisplayer* openDisplayer(wxWindow* parent, const wxString& filename);
	virtual wxString getFileWildcard() const;
	virtual wxString getDescription(const wxString& filename) const;
};

#endif /* IMGFORMATPROVIDER_H_ */
