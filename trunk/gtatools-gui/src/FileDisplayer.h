/*
 * FileDisplayer.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef FILEDISPLAYER_H_
#define FILEDISPLAYER_H_

#include <wx/wx.h>
#include <istream>
#include "DataSource.h"

using std::istream;

/**	\brief Represents a panel which can display special types of file formats.
 *
 * 	This is the component that is shown when a file with a certain file type is opened in some way. This
 * 	may either be by the 'Open' menu, or by embedded controls (such as in IMGs). Mapping from file types
 * 	to their correct FileDisplayers is done using FormatProviders.
 */
class FileDisplayer : public wxPanel {
public:
	FileDisplayer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL,
			const wxString& name = wxT("displayer"));
	virtual ~FileDisplayer() {};

	/**	\brief Should display the data from the given source.
	 *
	 * 	This includes closing the previously shown data, if any, and then calling doDisplay(). The given
	 * 	DataSource pointer will be kept internally and deleted when the file is closed.
	 *
	 *	@param source The source from which to read the file data.
	 *	@return The return value of doDisplay().
	 */
	bool display(DataSource* source);

	/**	\brief Should close the currently shown file.
	 *
	 * 	If there's nothing to close, this method simply does nothing. The DataSource* of the last displayed
	 * 	file will be automatically deleted when close() succeeds.
	 */
	void close();

public:

	/**	\brief Should display the given DataSource.
	 *
	 * 	This method may not be called directly. To display a file, display() is always called as a wrapper.
	 * 	Therefore, this method can be sure that it does not have to close the last file first.
	 *
	 *	@param source The source data to display.
	 *	@return true if displaying suceeded, false otherwise.
	 */
	virtual bool doDisplay(DataSource* source) = 0;

	/**	\brief Should close the current DataSource.
	 *
	 * 	This method may not be called directly. To close a file, close() is always called as a wrapper.
	 * 	Therefore, this method can be sure that there is really something to close when it is called.
	 */
	virtual void doClose() = 0;

private:
	DataSource* source;
	bool open;
};

#endif /* FILEDISPLAYER_H_ */
