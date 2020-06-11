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

#include "RWBSWidget.h"
#include "RWBSGUIModule.h"
#include "../../System.h"
#include "../../gui/HexEditorUndoCommand.h"
#include <gtaformats/img/IMGArchive.h>
#include <QSettings>
#include <QPushButton>



RWBSWidget::RWBSWidget(QWidget* parent)
		: QWidget(parent), model(new RWBSSectionModel), currentSection(NULL), openingSection(false), undoStack(NULL),
		  editorUndoDecorator(this)
{
	ui.setupUi(this);

	/*HexEditorBlock* blk1 = new HexEditorBlock(5, 7);
	blk1->setBackgroundBrush(QBrush(QColor(180, 0, 0, 100)));

	HexEditorBlock* blk2 = new HexEditorBlock(19, 37);
	blk2->setBackgroundBrush(QBrush(QColor(0, 0, 180, 100)));
	blk2->setBorderType(HexEditorBlock::ThinBorder);

	QList<int> subdivs;
	subdivs << 4 << 8 << 12 << 16;
	blk2->setSubdivisions(subdivs);

	HexEditorBlock* blk3 = new HexEditorBlock(9, 23);
	blk3->setBackgroundBrush(QBrush(QColor(0, 0, 180, 100)));
	blk3->setBorderType(HexEditorBlock::ThinBorder);

	editorDoc.addBlock(blk1);
	editorDoc.addBlock(blk2);
	editorDoc.addBlock(blk3);*/

	ui.editor->setDocument(&editorDoc);

	sectionTree = new RWBSSectionTree(model, ui.sectionTreeContainerWidget);
	ui.sectionTreeContainerWidget->layout()->addWidget(sectionTree);

	ui.editor->setUndoDecorator(&editorUndoDecorator);

	ui.decoder->connectToEditor(ui.editor);

	QSettings settings;
	ui.mainSplitter->restoreState(settings.value("gui_geometry_RWBSWidget/mainSplitter_state").toByteArray());
	ui.treeEditorSplitter->restoreState(settings.value("gui_geometry_RWBSWidget/treeEditorSplitter_state").toByteArray());

	setCurrentSection(NULL);
	sectionTree->setEnabled(false);

	setEditable(true);

	connect(ui.mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(mainSplitterValueChanged(int, int)));
	connect(ui.treeEditorSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(treeEditorSplitterValueChanged(int, int)));

	connect(sectionTree, SIGNAL(activated(const QModelIndex&)), this,
			SLOT(sectionActivated(const QModelIndex&)));
	connect(ui.editor, SIGNAL(dataReplaced(int, int, const QByteArray&, const QByteArray&)), this,
			SLOT(sectionDataReplaced(int, int, const QByteArray&, const QByteArray&)));

	connect(sectionTree, SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)), this,
			SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)));
	connect(sectionTree, SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)), this,
			SLOT(sectionUpdatedSlot(RWSection*, uint32_t, uint32_t, bool)));

	connect(sectionTree, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this,
			SLOT(sectionRemovedSlot(RWSection*)));
	connect(sectionTree, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this,
			SIGNAL(sectionRemoved(RWSection*, RWSection*)));

	connect(sectionTree, SIGNAL(sectionInserted(RWSection*)), this, SIGNAL(sectionInserted(RWSection*)));
}


RWBSWidget::~RWBSWidget()
{
	// TODO: Implement this...
}


void RWBSWidget::showEvent(QShowEvent* evt)
{
	/*RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
	guiModule->setEditor(ui.editor);
	System::getInstance()->installGUIModule(guiModule);*/
}


void RWBSWidget::hideEvent(QHideEvent* evt)
{
	/*RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
	guiModule->setEditor(NULL);
	System::getInstance()->uninstallGUIModule(guiModule);*/
}


void RWBSWidget::loadFile(const File& file)
{
	istream* stream = file.openInputStream(istream::binary);

	uint64_t offset = 0;

	QList<RWSection*> sects;

	RWSection* sect = NULL;
	while ((sect = RWSection::readSection(stream, sect))  !=  NULL) {
		sect->computeAbsoluteOffsets(offset);
		offset += sect->getSize() + 12;
		addRootSection(new RWSection(*sect));
		sects << sect;
	}

	for (QList<RWSection*>::iterator it = sects.begin() ; it != sects.end() ; it++) {
		delete (*it);
	}

	delete stream;
}


void RWBSWidget::addRootSection(RWSection* sect)
{
	/*model->addRootSection(sect);
	sectionTree->setModel(NULL);
	sectionTree->setModel(ui.sectionTree->getModel());*/

	if (undoStack) {
		undoStack->clear();
	}

	QUndoCommand* cmd = model->createAddRootSectionCommand(sect);
	cmd->redo();
	delete cmd;

	sectionTree->setModel(NULL);
	sectionTree->setModel(model);

	sectionTree->setEnabled(true);
}


void RWBSWidget::clear()
{
	if (undoStack) {
		undoStack->clear();
	}

	QUndoCommand* cmd = model->createClearCommand();
	cmd->redo();
	delete cmd;

	sectionTree->setModel(NULL);
	sectionTree->setModel(model);

	setCurrentSection(NULL);

	sectionTree->setEnabled(false);
}


void RWBSWidget::setCurrentSection(RWSection* sect)
{
	RWSection* oldSect = currentSection;

	currentSection = sect;

	editorUndoDecorator.blockUndo();

	if (sect) {
		openingSection = true;

		if (sect->isDataSection()) {
			ui.editor->setEnabled(true);
			ui.decoder->setEnabled(true);
			editorDoc.setData(QByteArray((const char*) sect->getData(), sect->getSize()));
		} else {
			editorDoc.setData(QByteArray());
			ui.editor->setEnabled(false);
			ui.decoder->setEnabled(false);
		}

		openingSection = false;
	} else {
		editorDoc.setData(QByteArray());
		ui.editor->setEnabled(false);
		ui.decoder->setEnabled(false);
	}

	editorUndoDecorator.unblockUndo();

	if (sect) {
		QModelIndex idx = model->getSectionIndex(sect);

		sectionTree->setCurrentIndex(idx);
	} else {
		sectionTree->setCurrentIndex(QModelIndex());
	}

	emit currentSectionChanged(oldSect, currentSection);
}


void RWBSWidget::save(const File& file)
{
	applyChanges();

	File pfile = file.getParent();

	if (pfile.isArchiveDirectory()) {
		IMGArchive img(pfile, IMGArchive::ReadWrite);

		IMGArchive::EntryIterator eit = img.getEntryByName(file.getPath().getFileName().get());

		if (eit != img.getEntryEnd()) {
			size_t size = 0;

			for (SectIterator it = getRootSectionBegin() ; it != getRootSectionEnd() ; it++) {
				RWSection* sect = *it;
				size += sect->getSize() + 12;
			}

			img.resizeEntry(eit, IMG_BYTES2BLOCKS(size));

			iostream* stream = (iostream*) img.gotoEntry(eit);

			for (SectIterator it = getRootSectionBegin() ; it != getRootSectionEnd() ; it++) {
				RWSection* sect = *it;
				sect->write(stream);
			}

			delete stream;

			img.sync();
		}
	} else {
		ostream* stream = file.openOutputStream(ostream::binary);

		for (SectIterator it = getRootSectionBegin() ; it != getRootSectionEnd() ; it++) {
			RWSection* sect = *it;
			sect->write(stream);
		}

		delete stream;
	}
}


void RWBSWidget::setEditable(bool editable)
{
	this->editable = editable;

	ui.editor->setEditable(editable);
	ui.decoder->setEditable(editable);
	sectionTree->setEditable(editable);
}


void RWBSWidget::applyChanges()
{
	if (currentSection) {
		if (currentSection->isDataSection()) {
			QByteArray data = ui.editor->getData();
			uint8_t* dataCpy = new uint8_t[data.size()];
			memcpy(dataCpy, data.constData(), data.size());
			currentSection->setData(dataCpy, data.size());
		}
	}
}


void RWBSWidget::sectionActivated(const QModelIndex& index)
{
	if (!index.isValid()) {
		return;
	}

	applyChanges();

	RWSection* sect = (RWSection*) index.internalPointer();
	setCurrentSection(sect);
}


void RWBSWidget::sectionDataReplaced(int offset, int len, const QByteArray& oldData, const QByteArray& newData)
{
	if (!openingSection)
		emit sectionDataChanged(currentSection);
}


void RWBSWidget::sectionRemovedSlot(RWSection* sect)
{
	if (currentSection == sect) {
		setCurrentSection(NULL);
	}
}


void RWBSWidget::sectionUpdatedSlot(RWSection* sect, uint32_t oldID, uint32_t oldVersion, bool oldContainer)
{
	if (oldContainer != sect->isContainerSection()) {
		setCurrentSection(sect);
	}
}


void RWBSWidget::mainSplitterValueChanged(int pos, int idx)
{
	QSettings settings;

	settings.setValue("gui_geometry_RWBSWidget/mainSplitter_state", ui.mainSplitter->saveState());
}


void RWBSWidget::treeEditorSplitterValueChanged(int pos, int idx)
{
	QSettings settings;

	settings.setValue("gui_geometry_RWBSWidget/treeEditorSplitter_state", ui.treeEditorSplitter->saveState());
}


void RWBSWidget::setUndoStack(QUndoStack* undoStack)
{
	this->undoStack = undoStack;
	ui.editor->setUndoStack(undoStack);
	sectionTree->setUndoStack(undoStack);
}
