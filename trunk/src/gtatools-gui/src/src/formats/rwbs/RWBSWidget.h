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

#ifndef RWBSWIDGET_H_
#define RWBSWIDGET_H_

#include <ui_RWBSWidget.h>
#include <gtaformats/util/File.h>
#include <gtaformats/rwbs/RWSection.h>
#include "RWBSSectionModel.h"
#include "RWBSSectionTree.h"
#include "../../BatchingUndoDecorator.h"
#include "../../gui/HexEditorDocument.h"
#include "RWBSAnalyzer.h"
#include <QtCore/QList>
#include <QtGui/QUndoStack>

extern "C" {

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

}



class RWBSWidget : public QWidget {
	Q_OBJECT

private:
	typedef QList<RWSection*> SectList;

	class SwitchingHexEditorUndoCommand : public QUndoCommand
	{
	public:
		SwitchingHexEditorUndoCommand(RWBSWidget* widget, QUndoCommand* cmd, RWBSSectionModel::PersistentRWSectionIndex sectIdx)
				: widget(widget), cmd(cmd), sectIdx(sectIdx)
		{
		}

		virtual ~SwitchingHexEditorUndoCommand()
		{
			delete cmd;
		}

		virtual void undo()
		{
			jumpToSection();
			cmd->undo();
		}

		virtual void redo()
		{
			jumpToSection();
			cmd->redo();
		}

	private:
		void jumpToSection()
		{
			RWSection* sect = sectIdx.resolve(widget->getModel());

			if (!widget->isCurrentSection(sect)) {
				widget->setCurrentSection(sect);
			}
		}

	private:
		RWBSWidget* widget;
		QUndoCommand* cmd;
		RWBSSectionModel::PersistentRWSectionIndex sectIdx;
	};

	class SwitchingUndoDecorator : public BatchingUndoDecorator
	{
	public:
		SwitchingUndoDecorator(RWBSWidget* widget)
				: widget(widget)
		{
		}

		virtual void push(QUndoCommand* cmd)
		{
			RWBSSectionModel::PersistentRWSectionIndex sectIdx(widget->getCurrentSection(), widget->getModel());
			SwitchingHexEditorUndoCommand* scmd = new SwitchingHexEditorUndoCommand(widget, cmd, sectIdx);
			BatchingUndoDecorator::push(scmd);
		}

	private:
		RWBSWidget* widget;
	};

public:
	typedef SectList::iterator SectIterator;
	typedef SectList::const_iterator ConstSectIterator;

public:
	RWBSWidget(QWidget* parent = NULL);
	~RWBSWidget();
	void addRootSection(RWSection* sect);
	void clear();
	void loadFile(const File& file);
	void save(const File& file);
	void applyChanges();
	SectIterator getRootSectionBegin() { return model->getRootSectionBegin(); }
	SectIterator getRootSectionEnd() { return model->getRootSectionEnd(); }
	ConstSectIterator getRootSectionBegin() const { return model->getRootSectionBegin(); }
	ConstSectIterator getRootSectionEnd() const { return model->getRootSectionEnd(); }
	int getRootSectionCount() const { return model->getRootSectionCount(); }
	void setEditable(bool editable);
	bool isEditable() const { return editable; }
	void setCurrentSection(RWSection* sect);
	RWSection* getCurrentSection() { return currentSection; }
	bool isCurrentSection(RWSection* sect) { return getCurrentSection() == sect; }
	void setUndoStack(QUndoStack* undoStack);
	QUndoStack* getUndoStack() { return undoStack; }
	const QUndoStack* getUndoStack() const { return undoStack; }
	RWBSSectionModel* getModel() { return model; }

protected:
	void showEvent(QShowEvent* evt);
	void hideEvent(QHideEvent* evt);

signals:
	void sectionInserted(RWSection* sect);
	void sectionRemoved(RWSection* sect, RWSection* oldParent);
	void sectionUpdated(RWSection* sect, uint32_t oldID, uint32_t oldVersion, bool oldContainer);
	void sectionDataChanged(RWSection* sect);
	void currentSectionChanged(RWSection* oldSect, RWSection* newSect);

private slots:
	void sectionActivated(const QModelIndex& index);
	void sectionDataReplaced(int offset, int len, const QByteArray& oldData, const QByteArray& newData);
	void sectionRemovedSlot(RWSection* sect);
	void sectionUpdatedSlot(RWSection* sect, uint32_t oldID, uint32_t oldVersion, bool oldContainer);
	void mainSplitterValueChanged(int pos, int idx);
	void treeEditorSplitterValueChanged(int pos, int idx);

private:
	Ui_RWBSWidget ui;
	RWBSSectionTree* sectionTree;
	RWBSSectionModel* model;
	RWSection* currentSection;
	bool openingSection;
	bool editable;
	QUndoStack* undoStack;
	SwitchingUndoDecorator editorUndoDecorator;
	HexEditorDocument editorDoc;
};

#endif /* RWBSWIDGET_H_ */
