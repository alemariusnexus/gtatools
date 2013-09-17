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

#include "RWBSWidget.h"
#include "RWBSGUIModule.h"
#include "../../System.h"



RWBSWidget::RWBSWidget(QWidget* parent)
		: QWidget(parent), currentSection(NULL), openingSection(false)
{
	ui.setupUi(this);

	ui.splitter->setSizes(QList<int>() << width()/4 << width()/4 * 3);

	ui.decoder->connectToEditor(ui.editor);

	connect(ui.sectionTree, SIGNAL(activated(const QModelIndex&)), this,
			SLOT(sectionActivated(const QModelIndex&)));
	connect(ui.editor, SIGNAL(dataChanged(const QByteArray&)), this,
			SLOT(sectionDataChanged(const QByteArray&)));

	connect(ui.sectionTree, SIGNAL(sectionChanged(RWSection*)), this, SIGNAL(sectionChanged(RWSection*)));

	connect(ui.sectionTree, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this,
			SLOT(sectionRemovedSlot(RWSection*)));
	connect(ui.sectionTree, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this,
			SIGNAL(sectionRemoved(RWSection*, RWSection*)));

	connect(ui.sectionTree, SIGNAL(sectionInserted(RWSection*)), this, SIGNAL(sectionInserted(RWSection*)));
}


void RWBSWidget::showEvent(QShowEvent* evt)
{
	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
	guiModule->setEditor(ui.editor);
	System::getInstance()->installGUIModule(guiModule);
}


void RWBSWidget::hideEvent(QHideEvent* evt)
{
	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
	guiModule->setEditor(NULL);
	System::getInstance()->uninstallGUIModule(guiModule);
}


void RWBSWidget::loadFile(const File& file)
{
	istream* stream = file.openInputStream(istream::binary);

	uint64_t offset = 0;

	RWSection* sect = NULL;
	while ((sect = RWSection::readSection(stream, sect))  !=  NULL) {
		addRootSection(sect);
		sect->computeAbsoluteOffsets(offset);
		offset += sect->getSize() + 12;
	}

	delete stream;
}


void RWBSWidget::addRootSection(RWSection* sect)
{
	ui.sectionTree->getModel()->addRootSection(sect);
}


void RWBSWidget::save(const File& file)
{
	applyChanges();

	File* pfile = file.getParent();

	if (pfile->isArchiveFile()) {
		IMGArchive img(*pfile, IMGArchive::ReadWrite);

		IMGArchive::EntryIterator eit = img.getEntryByName(file.getPath()->getFileName().get());

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

	delete pfile;
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
	currentSection = sect;

	if (sect) {
		openingSection = true;

		if (sect->isDataSection()) {
			ui.editor->setEnabled(true);
			ui.editor->setData(QByteArray((const char*) sect->getData(), sect->getSize()));
		} else {
			ui.editor->setData(QByteArray());
			ui.editor->setEnabled(false);
		}

		openingSection = false;
	}
}


void RWBSWidget::sectionDataChanged(const QByteArray& data)
{
	if (!openingSection)
		emit sectionChanged(currentSection);
}


void RWBSWidget::sectionRemovedSlot(RWSection* sect)
{
	if (currentSection == sect) {
		ui.editor->setData(QByteArray());
		ui.editor->setEnabled(false);
		currentSection = NULL;
	}
}
