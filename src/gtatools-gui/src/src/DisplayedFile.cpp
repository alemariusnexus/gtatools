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

#include "DisplayedFile.h"
#include "formats/EntityHandler.h"
#include "System.h"
#include <QtGui/QFileDialog>



DisplayedFile::DisplayedFile(const File& file, EntityHandler* handler, QWidget* widget)
		: DisplayedEntity(), file(file), widget(widget), handler(handler)
{
	signature = QByteArray("DisplayedFile");
	signature.append((const char*) handler, sizeof(const char*));
	signature.append(file.getPath()->toString().get());
}


DisplayedFile::~DisplayedFile()
{
	if (widget)
		delete widget;
}


bool DisplayedFile::doSaveChanges(bool letUserChooseFile)
{
	if (letUserChooseFile) {
		QString filter = handler->buildFileDialogFilter();

		QString fname = QFileDialog::getSaveFileName(System::getInstance()->getMainWindow(),
				tr("Select file"), file.getPath()->toString().get(), filter);

		if (!fname.isNull()) {
			File file(fname.toLocal8Bit().constData());
			this->file = file;

			DisplayedEntity::doSaveChanges(false);

			return true;
		}

		return false;
	} else {
		DisplayedEntity::doSaveChanges(false);

		return true;
	}
}


QString DisplayedFile::getFormatName() const
{
	return handler->getFileFormatName(file);
}
