/*
 * FileViewWidget.cpp
 *
 *  Created on: 24.09.2010
 *      Author: alemariusnexus
 */

#include "FileViewWidget.h"
#include "../FormatManager.h"



FileViewWidget::FileViewWidget(const FileOpenRequest& request)
{
	ui.setupUi(this);
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
