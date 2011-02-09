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
 */

#include "FileViewWidget.h"
#include "../config.h"
#include "../FormatManager.h"
#include <QtCore/QSettings>



FileViewWidget::FileViewWidget(const FileOpenRequest& request)
		: displayWidget(NULL)
{
	ui.setupUi(this);

	QSettings settings;
	bool compact = settings.value("gui/compact_mode", false).toBool();
	ui.fileNameLabel->setVisible(!compact);
	ui.fileInfoWidget->setVisible(!compact);

	File* file = request.getFile();

	ui.fileNameLabel->setText(QString(file->getPath()->getFileName()));

	if (file->isDirectory()) {
		ui.fileTypeLabel->setText(QString(tr("Directory")));
		ui.fileSizeLabel->setText(QString(tr("%1 files")).arg(file->getChildCount()));
	} else {
		File::filesize size = file->getSize();

		if (size > 1000000) {
			ui.fileSizeLabel->setText(QString(tr("%1MB")).arg(size/1000000.0f));
		} else if (size > 1000) {
			ui.fileSizeLabel->setText(QString(tr("%1kB")).arg(size/1000.0f));
		} else {
			ui.fileSizeLabel->setText(QString(tr("%1B")).arg(size));
		}

		QLinkedList<FormatHandler*> handlers = FormatManager::getInstance()->getHandlers(*file);

		if (handlers.size() == 0) {
			ui.fileTypeLabel->setText(QString(tr("Unrecognized File")));
		} else {
			ui.fileTypeLabel->setText((*handlers.begin())->getFormatName(file));
		}

		QLinkedList<FormatHandler*>::iterator it;

		for (it = handlers.begin() ; it != handlers.end() ; it++) {
			FormatHandler* handler = *it;

			displayWidget = handler->createWidgetForFile(request, ui.contentPluginWidget);

			if (displayWidget) {
				ui.contentPluginWidget->layout()->addWidget(displayWidget);
				break;
			}
		}

		/*if (contentTabCompact) {
			contentTabCompact->setTabEnabled(1, currentDisplayWidget != NULL);
		}*/
	}
}


FileViewWidget::~FileViewWidget()
{
	if (displayWidget) {
		delete displayWidget;
	}
}
