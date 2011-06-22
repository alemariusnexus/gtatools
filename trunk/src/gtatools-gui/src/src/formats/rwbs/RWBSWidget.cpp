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

#include "RWBSWidget.h"
#include "RWBSGUIModule.h"
#include "../../System.h"



RWBSWidget::RWBSWidget(QWidget* parent)
		: QWidget(parent), currentSection(NULL), openingSection(false)
{
	ui.setupUi(this);

	ui.splitter->setSizes(QList<int>() << width()/4 << width()/4 * 3);

	ui.sectionTree->setModel(&sectModel);

	ui.decoder->connectToEditor(ui.editor);

	connect(ui.sectionTree, SIGNAL(activated(const QModelIndex&)), this,
			SLOT(sectionActivated(const QModelIndex&)));
	connect(ui.editor, SIGNAL(dataChanged(const QByteArray&)), this,
			SLOT(sectionDataChanged(const QByteArray&)));
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
	rootSects << sect;
	sectModel.addRootSection(sect);
}


void RWBSWidget::save(const File& file)
{
	applyChanges();

	ostream* stream = file.openOutputStream(ostream::binary);

	for (int i = 0 ; i < rootSects.size() ; i++) {
		RWSection* sect = rootSects[i];
		sect->write(stream);
	}

	delete stream;
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
