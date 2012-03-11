/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef DEFAULTDISPLAYEDFILE_H_
#define DEFAULTDISPLAYEDFILE_H_

#include "DisplayedFile.h"


class FormatHandler;


class DefaultDisplayedFile : public DisplayedFile {
	Q_OBJECT

public:
	DefaultDisplayedFile(const File& file, FormatHandler* handler, QWidget* widget = NULL)
			: DisplayedFile(handler), file(file), widget(widget) {}
	~DefaultDisplayedFile();
	virtual QWidget* getWidget() const { return widget; }
	void setWidget(QWidget* widget) { this->widget = widget; }
	virtual File getFile() const { return file; }
	virtual QString getName() const { return file.getPath()->getFileName().get(); }
	virtual void saveTo(const File& file) { DisplayedFile::saveTo(file); this->file = file; }

private:
	File file;
	QWidget* widget;
};

#endif /* DEFAULTDISPLAYEDFILE_H_ */
