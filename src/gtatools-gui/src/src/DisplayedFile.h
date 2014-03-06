/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef DISPLAYEDFILE_H_
#define DISPLAYEDFILE_H_

#include "DisplayedEntity.h"
#include <gtaformats/util/File.h>


class EntityHandler;


class DisplayedFile : public DisplayedEntity {
	Q_OBJECT

public:
	DisplayedFile(const File& file, EntityHandler* handler, QWidget* widget);
	~DisplayedFile();
	virtual QString getFormatName() const;
	virtual QWidget* getWidget() const { return widget; }
	void setWidget(QWidget* widget) { this->widget = widget; }
	virtual File getFile() const { return file; }
	virtual QString getName() const { return file.getPath()->getFileName().get(); }
	EntityHandler* getEntityHandler() { return handler; }
	virtual QByteArray getSignature() const { return signature; }

protected:
	virtual bool doSaveChanges(bool letUserChooseFile);

private:
	File file;
	QWidget* widget;
	EntityHandler* handler;
	QByteArray signature;
};

#endif /* DISPLAYEDFILE_H_ */
