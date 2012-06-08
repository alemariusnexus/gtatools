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

#include "FileViewWidget.h"
#include <gtatools-gui/config.h>
#include "../formats/FormatManager.h"
#include <QtCore/QSettings>



FileViewWidget::FileViewWidget(DisplayedFile* dfile)
		: dfile(dfile), displayWidget(NULL)
{
	ui.setupUi(this);

	QSettings settings;
	bool compact = settings.value("gui/compact_mode", false).toBool();
	ui.fileNameLabel->setVisible(!compact);
	ui.fileInfoWidget->setVisible(!compact);

	File file = dfile->getFile();

	updateFile();

	if (!file.isDirectory()) {
		displayWidget = dfile->getWidget();

		if (displayWidget) {
			ui.contentPluginWidget->layout()->addWidget(displayWidget);
		}
	}

	connect(dfile, SIGNAL(saved()), this, SLOT(saved()));
}


FileViewWidget::~FileViewWidget()
{
	// It is DisplayedFile's responsibility to delete the widget.
	ui.contentPluginWidget->layout()->removeWidget(displayWidget);
	displayWidget->setParent(NULL);
}


void FileViewWidget::updateFile()
{
	File file = dfile->getFile();

	ui.fileNameLabel->setText(QString(file.getPath()->getFileName().get()));

	if (file.isDirectory()) {
		ui.fileTypeLabel->setText(QString(tr("Directory")));
		ui.fileSizeLabel->setText(QString(tr("%1 files")).arg(file.getChildCount()));
	} else {
		File::filesize size = file.getSize();

		if (size > 1000000) {
			ui.fileSizeLabel->setText(QString(tr("%1MB")).arg(size/1000000.0f));
		} else if (size > 1000) {
			ui.fileSizeLabel->setText(QString(tr("%1kB")).arg(size/1000.0f));
		} else {
			ui.fileSizeLabel->setText(QString(tr("%1B")).arg(size));
		}

		ui.fileTypeLabel->setText(dfile->getFormatName());
	}
}


void FileViewWidget::saved()
{
	updateFile();
}
