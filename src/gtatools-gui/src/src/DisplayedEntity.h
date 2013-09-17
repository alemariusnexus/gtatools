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

#ifndef DISPLAYEDENTITY_H_
#define DISPLAYEDENTITY_H_

#include <QtCore/QString>
#include <QtGui/QWidget>


class DisplayedEntity : public QObject {
	Q_OBJECT

public:
	DisplayedEntity();
	virtual QWidget* getWidget() const = 0;
	virtual QString getName() const = 0;
	virtual bool canSave() const { return false; }
	virtual bool hasChanges() const { return changes; }
	virtual void setHasChanges(bool ch)
			{ bool old = changes; changes = ch; if (old != ch) emit changeStatusChanged(); }
	virtual bool save(bool useLast);

private:
	virtual bool doSave(bool useLast) { return false; }

protected:
	void emitChangedStatusChanged() { emit changeStatusChanged(); }

signals:
	void closed();
	void madeCurrent();
	void lostCurrent(DisplayedEntity* newCurrent);
	void saved();
	void changeStatusChanged();

private slots:
	void entityClosed(DisplayedEntity* file);
	void currentEntityChanged(DisplayedEntity* current, DisplayedEntity* prev);

private:
	bool changes;
};

#endif /* DISPLAYEDENTITY_H_ */
