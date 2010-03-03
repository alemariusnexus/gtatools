/*
 * FormatProvider.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef FORMATPROVIDER_H_
#define FORMATPROVIDER_H_

#include "FileDisplayer.h"
#include "DataSource.h"

/**	\brief Provides recognizing and loading functionality to the program for a specific format.
 *
 * 	To add new formats that can be loaded and/or recognized, you have to subclass this class and implement
 * 	the virtual methods. Also, you have to register your FormatProvider in the constructor of the
 * 	DisplayManager to make it available to the system.
 *
 * 	FormatProviders don't have to implement the format. If they return false in isImplemented(), they
 * 	may return NULL in openDisplayer(). If they do, the format will be recognized (and thus for example
 * 	the file type of a matching entry inside IMG files can be displayed), but can't be opened.
 */
class FormatProvider {
public:

	/**	\brief Should return true if this FormatProvider recognizes the format of the given file name.
	 *
	 * 	You may not assume that filename refers to a real file. It could also be an entry inside an archive
	 * 	file, so you should only make assumptions on file name or extension (IMG for example).
	 *
	 *	@param filename The name of the file (not necessarily a real file!)
	 *	@return true if this provider recognizes the file's format.
	 */
	virtual bool canDisplay(const wxString& filename) const = 0;

	/**	\brief Should open a displayer for the given source data.
	 *
	 * 	This method must return a non-NULL value if isImplemented() returns true. Then, a new FileDisplayer
	 * 	must be created and returned which displays the given source (displaying must be done here, too!).
	 * 	If isImplemented() returns false, then this may return NULL.
	 *
	 *	@param parent The parent window to which the FileDisplayer should be added.
	 *	@param source The data source that should be displayed.
	 *	@return A new, open FileDisplayer or NULL, if isImplemented() is false.
	 */
	virtual FileDisplayer* openDisplayer(wxWindow* parent, DataSource* source) = 0;

	/**	\brief Should return the file wildcard for this format as needed by wxWidgets file displayer dialogs.
	 *
	 * 	This, currently, is ignored if isImplemented() is false as unimplemented files may not be opened.
	 *
	 *	@return The wildcard string for the format.
	 */
	virtual wxString getFileWildcard() const = 0;

	/**	\brief Should return a brief description of the format type.
	 *
	 * 	This string is used, for example, in the 'Type' label of IMG entries.
	 *
	 * @param filename The name of the file to get the description for.
	 * @return A brief description/name of the file format.
	 */
	virtual wxString getDescription(const wxString& filename) const = 0;

	/**	\brief Should return whether this FormatProvider is able to open/load it's recognized file formats.
	 *
	 * 	When this is overriden to return false, then the format type is not displayed in the 'Open' dialog
	 * 	and is generally not displayable. Unimplemented formats can, anyway, be recognized so that it
	 * 	is possible to display at least their type.
	 *
	 *	@return Whether the FormatProvider can open a displayer for it's formats.
	 */
	virtual bool isImplemented() const { return true; }
};

#endif /* FORMATPROVIDER_H_ */
