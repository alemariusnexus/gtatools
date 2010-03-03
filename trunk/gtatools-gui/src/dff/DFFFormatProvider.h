/*
 * DFFFormatProvider.h
 *
 *  Created on: 31.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFFORMATPROVIDER_H_
#define DFFFORMATPROVIDER_H_

#include "../FormatProvider.h"

/**	\brief This is the implementation of a DFF format provider.
 *
 *	Currently, it is not implemented. This means that opening a displayer does not work, but it recognizes
 *	the DFF file format so that it can display it's file type in IMG files.
 */
class DFFFormatProvider : public FormatProvider {
public:
	/**	\brief Overriden method to return true only when the file ends in .dff.
	 *
	 *	@param filename The file name.
	 *	@return Whether the file name is a valid DFF file name (ends in .dff).
	 */
	virtual bool canDisplay(const wxString& filename) const;

	/**	\brief Currently not supported.
	 *
	 *	@param parent
	 *	@param stream
	 *	@return
	 */
	virtual FileDisplayer* openDisplayer(wxWindow* parent, DataSource* stream);

	/**	\brief Returns the DFF wildcard.
	 *
	 *	@return The DFF file name wildcard.
	 */
	virtual wxString getFileWildcard() const;

	/**	\brief Returns a language-dependend description.
	 *
	 *	@param filename The file name, unused here.
	 *	@return The language-dependend description.
	 */
	virtual wxString getDescription(const wxString& filename) const;

	/**	\brief Returns false, as DFF is not implemented yet.
	 *
	 *	@return Currently always false.
	 */
	virtual bool isImplemented() const { return true; }
};

#endif /* DFFFORMATPROVIDER_H_ */
