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

#ifndef RWBSWIDGET_H_
#define RWBSWIDGET_H_

#include <ui_RWBSWidget.h>
#include <gtaformats/util/File.h>
#include <gtaformats/rwbs/RWSection.h>
#include "RWBSSectionModel.h"
#include <QtCore/QList>



class RWBSWidget : public QWidget {
	Q_OBJECT

private:
	typedef QList<RWSection*> SectList;

public:
	typedef SectList::iterator SectIterator;
	typedef SectList::const_iterator ConstSectIterator;

public:
	RWBSWidget(QWidget* parent = NULL);
	void addRootSection(RWSection* sect);
	void loadFile(const File& file);
	void save(const File& file);
	SectIterator getRootSectionBegin() { return ui.sectionTree->getModel()->getRootSectionBegin(); }
	SectIterator getRootSectionEnd() { return ui.sectionTree->getModel()->getRootSectionEnd(); }
	ConstSectIterator getRootSectionBegin() const { return ui.sectionTree->getModel()->getRootSectionBegin(); }
	ConstSectIterator getRootSectionEnd() const { return ui.sectionTree->getModel()->getRootSectionEnd(); }
	int getRootSectionCount() const { return ui.sectionTree->getModel()->getRootSectionCount(); }

protected:
	void showEvent(QShowEvent* evt);
	void hideEvent(QHideEvent* evt);

private:
	void applyChanges();

signals:
	void sectionChanged(RWSection* sect);
	void sectionInserted(RWSection* sect);
	void sectionRemoved(RWSection* sect, RWSection* oldParent);

private slots:
	void sectionActivated(const QModelIndex& index);
	void sectionDataChanged(const QByteArray& data);
	void sectionRemovedSlot(RWSection* sect);

private:
	Ui_RWBSWidget ui;
	RWSection* currentSection;
	bool openingSection;
};

#endif /* RWBSWIDGET_H_ */
