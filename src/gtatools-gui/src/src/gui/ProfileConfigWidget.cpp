/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "ProfileConfigWidget.h"
#include <qfiledialog.h>
#include <gtaformats/util/File.h>



ProfileConfigWidget::ProfileConfigWidget(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.fileAddButton, SIGNAL(clicked(bool)), this, SLOT(fileAddButtonClicked(bool)));
	connect(ui.dirAddButton, SIGNAL(clicked(bool)), this, SLOT(dirAddButtonClicked(bool)));
	connect(ui.fileEditButton, SIGNAL(clicked(bool)), this, SLOT(fileEditButtonClicked(bool)));
	connect(ui.fileRemoveButton, SIGNAL(clicked(bool)), this, SLOT(fileRemoveButtonClicked(bool)));
	connect(ui.fileList, SIGNAL(currentRowChanged(int)), this, SLOT(currentResourceChanged(int)));
}


ProfileConfigWidget::~ProfileConfigWidget()
{
	clearFiles();
}


void ProfileConfigWidget::getFiles(QLinkedList<QString>& dest)
{
	for (int i = 0 ; i < ui.fileList->count() ; i++) {
		QListWidgetItem* item = ui.fileList->item(i);
		dest << item->text();
	}
}


void ProfileConfigWidget::clearFiles()
{
	int count = ui.fileList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.fileList->takeItem(0);
	}
}


void ProfileConfigWidget::displayProfile(Profile* profile)
{
	clearFiles();
	ui.nameField->setText(profile->getName());

	Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		new QListWidgetItem((*it)->getPath()->toString(), ui.fileList);
	}
}


void ProfileConfigWidget::fileAddButtonClicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select one or more resources"));
	QStringList filesCpy = files;
	QStringList::iterator it;

	//if (!fname.isNull()) {
	for (it = filesCpy.begin() ; it != filesCpy.end() ; it++) {
		new QListWidgetItem(*it, ui.fileList);
	}
}


void ProfileConfigWidget::dirAddButtonClicked(bool checked)
{
	QString fname = QFileDialog::getExistingDirectory(this, tr("Select a resource directory"));

	if (!fname.isNull()) {
		//ui.fileList->addItem(fname);
		new QListWidgetItem(fname, ui.fileList);
	}
}


void ProfileConfigWidget::fileEditButtonClicked(bool checked)
{
	QListWidgetItem* item = ui.fileList->currentItem();

	if (!item) {
		return;
	}

	QString fname = item->text();

	if (File(fname.toLocal8Bit().constData()).isDirectory()) {
		QString newFname = QFileDialog::getExistingDirectory(this, tr("Select the new resource directory"),
				fname);

		if (!newFname.isNull()) {
			item->setText(newFname);
		}
	} else {
		QString newFname = QFileDialog::getOpenFileName(this, tr("Select the new resource file"), fname);

		if (!newFname.isNull()) {
			item->setText(newFname);
		}
	}
}


void ProfileConfigWidget::fileRemoveButtonClicked(bool checked)
{
	QListWidgetItem* item = ui.fileList->takeItem(ui.fileList->currentRow());

	if (!item) {
		return;
	}

	delete item;
}


void ProfileConfigWidget::currentResourceChanged(int index)
{
	ui.fileRemoveButton->setEnabled(index != -1);
	ui.fileEditButton->setEnabled(index != -1);
}
