/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include <QtCore/QString>
#include <QtGui/QWidget>
#include <gtaformats/util/File.h>

class FormatHandler;


class DisplayedFile : public QObject {
	Q_OBJECT

public:
	DisplayedFile(FormatHandler* handler);
	//virtual ~DisplayedFile();
	virtual FormatHandler* getFormatHandler() { return handler; }
	virtual QWidget* getWidget() const = 0;
	virtual File getFile() const = 0;
	virtual QString getName() const = 0;
	virtual QString getFormatName() const;
	virtual bool canSave() const { return false; }
	virtual bool hasChanges() const { return changes; }
	virtual void setHasChanges(bool ch)
			{ bool old = changes; changes = ch; if (old != ch) emit changeStatusChanged(); }
	virtual void saveTo(const File& file) { if (canSave()) { emit saved(file); setHasChanges(false); } }

protected:
	void emitChangedStatusChanged() { emit changeStatusChanged(); }

signals:
	void closed();
	void madeCurrent();
	void lostCurrent(DisplayedFile* newCurrent);
	void saved(const File& file);
	void changeStatusChanged();

private slots:
	void fileClosed(DisplayedFile* file);
	void currentFileChanged(DisplayedFile* current, DisplayedFile* prev);

private:
	FormatHandler* handler;
	bool changes;
};

#endif /* DISPLAYEDFILE_H_ */
