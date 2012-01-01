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

#ifndef RWBSSECTIONMODEL_H_
#define RWBSSECTIONMODEL_H_

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QList>
#include <gtaformats/rwbs/RWSection.h>



class RWBSSectionModel : public QAbstractItemModel {
	Q_OBJECT

private:
	typedef QList<RWSection*> SectList;

public:
	typedef SectList::iterator SectIterator;
	typedef SectList::const_iterator ConstSectIterator;

public:
	void addRootSection(RWSection* sect);
	void removeRootSection(RWSection* sect);
	void insertSection(RWSection* sect, RWSection* parent = NULL, int index = -1);
	void applySectionChanges(RWSection* sect)
			{ QModelIndex idx = getSectionIndex(sect); emit dataChanged(idx, idx); }
	int removeSection(RWSection* sect);
	SectIterator getRootSectionBegin() { return rootSects.begin(); }
	SectIterator getRootSectionEnd() { return rootSects.end(); }
	ConstSectIterator getRootSectionBegin() const { return rootSects.begin(); }
	ConstSectIterator getRootSectionEnd() const { return rootSects.end(); }
	int getRootSectionCount() const { return rootSects.size(); }
	QModelIndex getSectionIndex(RWSection* sect);
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex& index) const;

signals:
	void sectionRemoved(RWSection* sect, RWSection* oldParent);
	void sectionInserted(RWSection* sect);

public:
	SectList rootSects;
};

#endif /* RWBSSECTIONMODEL_H_ */
