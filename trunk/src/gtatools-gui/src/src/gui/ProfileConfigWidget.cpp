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

#include "ProfileConfigWidget.h"
#include <QtGui/QFileDialog>
#include <gtaformats/util/File.h>



ProfileConfigWidget::ProfileConfigWidget(Profile* profile, QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.fileAddButton, SIGNAL(clicked(bool)), this, SLOT(fileAddButtonClicked(bool)));
	connect(ui.dirAddButton, SIGNAL(clicked(bool)), this, SLOT(dirAddButtonClicked(bool)));
	connect(ui.fileEditButton, SIGNAL(clicked(bool)), this, SLOT(fileEditButtonClicked(bool)));
	connect(ui.fileRemoveButton, SIGNAL(clicked(bool)), this, SLOT(fileRemoveButtonClicked(bool)));
	connect(ui.fileList, SIGNAL(itemSelectionChanged()), this, SLOT(resourceSelectionChanged()));

	connect(ui.datFileAddButton, SIGNAL(clicked(bool)), this, SLOT(datFileAddButtonClicked(bool)));
	connect(ui.datFileEditButton, SIGNAL(clicked(bool)), this, SLOT(datFileEditButtonClicked(bool)));
	connect(ui.datFileRemoveButton, SIGNAL(clicked(bool)), this, SLOT(datFileRemoveButtonClicked(bool)));
	connect(ui.datFileList, SIGNAL(itemSelectionChanged()), this, SLOT(datSelectionChanged()));

	connect(ui.datRootChooseButton, SIGNAL(clicked(bool)), this, SLOT(datRootChooseButtonClicked(bool)));

	displayedProfile = profile;
	clearFiles();

	ui.nameField->setText(profile->getName());
	ui.datRootField->setText(profile->getDATRootDirectory());

	Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		new QListWidgetItem((*it)->getPath()->toString(), ui.fileList);
	}

	for (it = profile->getDATFilesBegin() ; it != profile->getDATFilesEnd() ; it++) {
		new QListWidgetItem((*it)->getPath()->toString(), ui.datFileList);
	}

	ui.fileAddButton->setEnabled(profile != NULL);
	ui.dirAddButton->setEnabled(profile != NULL);
	ui.fileEditButton->setEnabled(profile != NULL);
	ui.fileRemoveButton->setEnabled(profile != NULL);

	ui.datFileAddButton->setEnabled(profile != NULL);
	ui.datFileEditButton->setEnabled(profile != NULL);
	ui.datFileRemoveButton->setEnabled(profile != NULL);

	if (profile)
		tryDATAutocomplete();
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


void ProfileConfigWidget::getDATFiles(QLinkedList<QString>& dest)
{
	for (int i = 0 ; i < ui.datFileList->count() ; i++) {
		QListWidgetItem* item = ui.datFileList->item(i);
		dest << item->text();
	}
}


void ProfileConfigWidget::clearFiles()
{
	int count = ui.fileList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.fileList->takeItem(0);
	}

	int datCount = ui.datFileList->count();

	for (int i = 0 ; i < datCount ; i++) {
		delete ui.datFileList->takeItem(0);
	}
}


void ProfileConfigWidget::fileAddButtonClicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select one or more resources"));
	QStringList filesCpy = files;
	QStringList::iterator it;

	for (it = filesCpy.begin() ; it != filesCpy.end() ; it++) {
		new QListWidgetItem(*it, ui.fileList);
	}

	tryDATAutocomplete();
}


void ProfileConfigWidget::dirAddButtonClicked(bool checked)
{
	QString fname = QFileDialog::getExistingDirectory(this, tr("Select a resource directory"));

	if (!fname.isNull()) {
		new QListWidgetItem(fname, ui.fileList);
	}

	tryDATAutocomplete();
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

	tryDATAutocomplete();
}


void ProfileConfigWidget::fileRemoveButtonClicked(bool checked)
{
	QList<QListWidgetItem*> items = ui.fileList->selectedItems();

	for (int i = 0 ; i < items.count() ; i++) {
		QListWidgetItem* item = items[i];
		delete item;
	}

	tryDATAutocomplete();
}


void ProfileConfigWidget::datFileAddButtonClicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select one or more DAT files"), QString(),
			tr("DAT Files (*.dat)"));
	QStringList filesCpy = files;
	QStringList::iterator it;

	for (it = filesCpy.begin() ; it != filesCpy.end() ; it++) {
		new QListWidgetItem(*it, ui.datFileList);
	}
}


void ProfileConfigWidget::datFileEditButtonClicked(bool checked)
{
	QListWidgetItem* item = ui.datFileList->currentItem();

	if (!item) {
		return;
	}

	QString fname = item->text();
	QString newFname = QFileDialog::getOpenFileName(this, tr("Select the new DAT file"), fname);

	if (!newFname.isNull()) {
		item->setText(newFname);
	}
}


void ProfileConfigWidget::datFileRemoveButtonClicked(bool checked)
{
	QList<QListWidgetItem*> items = ui.datFileList->selectedItems();

	for (int i = 0 ; i < items.count() ; i++) {
		QListWidgetItem* item = items[i];
		delete item;
	}
}


void ProfileConfigWidget::resourceSelectionChanged()
{
	int numSel = ui.fileList->selectedItems().count();

	ui.fileRemoveButton->setEnabled(numSel > 0);
	ui.fileEditButton->setEnabled(numSel == 1);
}


void ProfileConfigWidget::datSelectionChanged()
{
	int numSel = ui.datFileList->selectedItems().count();

	ui.datFileRemoveButton->setEnabled(numSel > 0);
	ui.datFileEditButton->setEnabled(numSel == 1);
}


void ProfileConfigWidget::datRootChooseButtonClicked(bool checked)
{
	QString dname = QFileDialog::getExistingDirectory(this, tr("Choose a DAT root directory"));

	if (!dname.isNull()) {
		ui.datRootField->setText(dname);
	}

	tryDATAutocomplete();
}


void ProfileConfigWidget::tryDATAutocomplete()
{
	// Try to autocomplete the DAT root directory
	if (ui.datRootField->text().isEmpty()) {
		for (int i = 0 ; i < ui.fileList->count() ; i++) {
			QString res = ui.fileList->item(i)->text();

			if (File(res.toLocal8Bit().constData()).isDirectory()) {
				ui.datRootField->setText(res);
				break;
			}
		}
	}

	// Try to autocomplete the DAT files
	if (ui.datFileList->count() == 0) {
		File datRoot(ui.datRootField->text().toLocal8Bit().constData());

		if (datRoot.exists()  &&  datRoot.isDirectory()) {
			FileIterator* it = (datRoot.getIterator());
			File* child;

			while ((child = it->next())  !=  NULL) {
				if (QString(child->getPath()->getFileName()).toLower() == QString("gta_sa.exe")) {
					new QListWidgetItem(File(datRoot, "data/default.dat").getPath()->toString(),
							ui.datFileList);
					new QListWidgetItem(File(datRoot, "data/gta.dat").getPath()->toString(),
							ui.datFileList);
					delete child;
					break;
				}

				delete child;
			}

			delete it;
		}
	}
}
