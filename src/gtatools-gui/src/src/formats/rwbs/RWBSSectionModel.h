/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <QtCore/QLinkedList>
#include <QtCore/QSet>
#include <QUndoCommand>
#include <gtaformats/rwbs/RWSection.h>
#include "../../CompoundUndoCommand.h"



class RWBSSectionModel : public QAbstractItemModel {
	Q_OBJECT

public:
	class PersistentRWSectionIndex
	{
	public:
		PersistentRWSectionIndex(RWSection* sect, RWBSSectionModel* model)
		{
			if (sect) {
				while (sect->getParent()) {
					path.prepend(sect->getParent()->indexOf(sect));
					sect = sect->getParent();
				}

				path.prepend(model->getRootSectionIndex(sect));
			}
		}

		PersistentRWSectionIndex(const PersistentRWSectionIndex& other)
		{
			path = other.path;
		}

		RWSection* resolve(RWBSSectionModel* model)
		{
			QLinkedList<unsigned int>::iterator pit = path.begin();

			if (pit == path.end())
				return NULL;

			RWSection* sect = *(model->getRootSectionBegin()+*pit);
			pit++;

			for (; pit != path.end() ; pit++) {
				sect = sect->getChildByIndex(*pit);
			}

			return sect;
		}

	private:
		QLinkedList<unsigned int> path;
	};


private:
	class RWBSSectionInsertRemoveCommand : public QUndoCommand
	{
	protected:
		RWBSSectionInsertRemoveCommand(RWBSSectionModel* model, RWSection* parentSect, RWSection* sect, int index)
				: model(model), parentIdx(PersistentRWSectionIndex(parentSect, model)), sect(sect),
				  index(index == -1 ? parentSect->getChildCount() : index)
		{
		}

		~RWBSSectionInsertRemoveCommand()
		{
			delete sect;
		}

		void remove()
		{
			RWSection* parent = parentIdx.resolve(model);
			RWSection* child;

			if (parent)
				child = parent->getChildByIndex(index);
			else
				child = model->rootSects[index];

			model->removeSection(child);
			delete child;
		}

		void insert()
		{
			RWSection* parent = parentIdx.resolve(model);
			model->insertSection(new RWSection(*sect), parent, index);
		}

	private:
		RWBSSectionModel* model;
		PersistentRWSectionIndex parentIdx;
		RWSection* sect;
		int index;
	};


	class RWBSSectionInsertCommand : public RWBSSectionInsertRemoveCommand
	{
	public:
		RWBSSectionInsertCommand(RWBSSectionModel* model, RWSection* parentSect, RWSection* sect, int index)
				: RWBSSectionInsertRemoveCommand(model, parentSect, sect, index)
		{
		}

		virtual void redo() { insert(); }
		virtual void undo() { remove(); }
	};


	class RWBSSectionRemoveCommand : public RWBSSectionInsertRemoveCommand
	{
	public:
		RWBSSectionRemoveCommand(RWBSSectionModel* model, RWSection* parentSect, int index)
				: RWBSSectionInsertRemoveCommand(model, parentSect,
						new RWSection(parentSect ? *parentSect->getChildByIndex(index) : *model->rootSects[index]), index)
		{
		}

		virtual void redo() { remove(); }
		virtual void undo() { insert(); }
	};


	class RWBSSectionUpdateCommand : public QUndoCommand
	{
	public:
		RWBSSectionUpdateCommand(RWBSSectionModel* model, RWSection* sect, uint32_t id, uint32_t version, bool container)
				: model(model), sectIdx(PersistentRWSectionIndex(sect, model)), id(id), version(version), container(container)
		{
		}

		virtual void redo() { toggle(); }
		virtual void undo() { toggle(); }

	private:
		void toggle()
		{
			RWSection* sect = sectIdx.resolve(model);

			uint32_t oldId = sect->getID();
			uint32_t oldVer = sect->getVersion();
			bool oldCont = sect->isContainerSection();

			model->updateSection(sect, id, version, container);

			id = oldId;
			version = oldVer;
			container = oldCont;
		}

	private:
		RWBSSectionModel* model;
		PersistentRWSectionIndex sectIdx;
		uint32_t id, version;
		bool container;
	};

private:
	typedef QList<RWSection*> SectList;

public:
	typedef SectList::iterator SectIterator;
	typedef SectList::const_iterator ConstSectIterator;

public:
	QUndoCommand* createInsertSectionCommand(RWSection* sect, RWSection* parent = NULL, int index = -1);
	QUndoCommand* createRemoveSectionCommand(RWSection* sect);
	QUndoCommand* createAddRootSectionCommand(RWSection* sect) { return createInsertSectionCommand(sect, NULL, -1); }
	QUndoCommand* createClearCommand();
	QUndoCommand* createUpdateSectionCommand(RWSection* sect, uint32_t id, uint32_t version, bool container);
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

private:
	int getRootSectionIndex(RWSection* sect) const { return rootSects.indexOf(sect, 0); }
	void addRootSection(RWSection* sect);
	void removeRootSection(RWSection* sect);
	void insertSection(RWSection* sect, RWSection* parent = NULL, int index = -1);
	void applySectionChanges(RWSection* sect)
			{ QModelIndex idx = getSectionIndex(sect); emit dataChanged(idx, idx); }
	int removeSection(RWSection* sect);
	void updateSection(RWSection* sect, uint32_t id, uint32_t version, bool container);
	void buildModelIndexListRecurse(QList<QModelIndex>& indices, QModelIndex idx);

signals:
	void sectionRemoved(RWSection* sect, RWSection* oldParent);
	void sectionInserted(RWSection* sect);
	void sectionUpdated(RWSection* sect, uint32_t oldID, uint32_t oldVersion, bool oldContainer);

public:
	SectList rootSects;
};

#endif /* RWBSSECTIONMODEL_H_ */
