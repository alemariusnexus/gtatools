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

#ifndef RWBSSECTIONTREE_H_
#define RWBSSECTIONTREE_H_

#include <QtGui/QTreeView>
#include <QtGui/QAction>
#include "RWBSSectionModel.h"



class RWBSSectionTree : public QTreeView {
	Q_OBJECT

public:
	RWBSSectionTree(QWidget* parent = NULL);
	RWBSSectionModel* getModel() { return &model; }

private slots:
	void contextMenuRequested(const QPoint& pos);
	void addRequested(bool);
	void removeRequested(bool);
	void editRequested(bool);

signals:
	void sectionChanged(RWSection* sect);
	void sectionRemoved(RWSection* sect, RWSection* oldParent);
	void sectionInserted(RWSection* sect);

private:
	RWBSSectionModel model;
	QAction* addAction;
	QAction* removeAction;
	QAction* editAction;
	RWSection* contextSect;
};

#endif /* RWBSSECTIONTREE_H_ */
