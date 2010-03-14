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

#ifndef DISPLAYMANAGER_H_
#define DISPLAYMANAGER_H_

#include "FileDisplayer.h"
#include "FormatProvider.h"
#include <wx/wx.h>
#include <istream>

using std::istream;


/**	\brief The register of FormatProviders.
 *
 * 	All FormatProviders are linked to the system here. Use this class to query FormatProviders. New
 * 	providers can be added in the constructor code. This class is a singleton. You can obtain the shared
 * 	instance with getInstance().
 *
 * 	\see FormatProvider
 */
class DisplayManager {
public:
	/**	\brief Returns the shared DisplayManager instance. */
	static DisplayManager* getInstance();

public:
	/**	\brief Deletes the shared DisplayManager.
	 *
	 * 	This deletes all registered FormatProviders as well.
	 */
	~DisplayManager();

	/**	\brief Returns a FormatProvider which can display the file with the given name.
	 *
	 * 	This will lead to a call to each FormatProvider's FormatProvider::canDisplay() method. Therefore,
	 * 	filename does not have to be a really existant file.
	 *
	 *	@param filename The file name for which to find a FormatProvider.
	 *	@return The first matching FormatProvider or NULL, if none was found.
	 */
	FormatProvider* getFormatProvider(const wxString& filename);

	/**	\brief Closes a FileDisplayer by calling it's FileDisplayer::close() method and deleting it.
	 *
	 *	@param displayer The displayer to close.
	 */
	void closeDisplayer(FileDisplayer* displayer);

	/**	\brief Returns an array of all registered FormatProviders.
	 *
	 *	@return An array of all registered FormatProviders.
	 */
	FormatProvider** getFormatProviders();

	/**	\brief Returns the number of registered FormatProviders.
	 *
	 *	@return The number of registered FormatProviders.
	 */
	short getFormatProviderCount() { return numProviders; }

private:
	/**	\brief Creates the one and only shared DisplayManager instance. */
	DisplayManager();

private:
	FormatProvider** providers;
	short numProviders;
};

#endif /* DISPLAYMANAGER_H_ */
