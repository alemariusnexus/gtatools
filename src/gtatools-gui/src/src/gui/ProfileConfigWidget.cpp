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

	ui.versionBox->addItem(tr("GTA San Andreas"), GameInfo::GTASA);
	ui.versionBox->addItem(tr("GTA Vice City"), GameInfo::GTAVC);
	ui.versionBox->addItem(tr("GTA III"), GameInfo::GTAIII);

	connect(ui.rootDirButton, SIGNAL(clicked(bool)), this, SLOT(rootDirButtonClicked(bool)));
	connect(ui.versionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(versionBoxChanged(int)));

	connect(ui.engineFileAddButton, SIGNAL(clicked(bool)), this, SLOT(engineFileAddButtonClicked(bool)));
	connect(ui.engineDirAddButton, SIGNAL(clicked(bool)), this, SLOT(engineDirAddButtonClicked(bool)));
	connect(ui.engineFileEditButton, SIGNAL(clicked(bool)), this, SLOT(engineFileEditButtonClicked(bool)));
	connect(ui.engineFileRemoveButton, SIGNAL(clicked(bool)), this, SLOT(engineFileRemoveButtonClicked(bool)));
	connect(ui.engineFileList, SIGNAL(itemSelectionChanged()), this, SLOT(engineResourceSelectionChanged()));

	connect(ui.searchFileAddButton, SIGNAL(clicked(bool)), this, SLOT(searchFileAddButtonClicked(bool)));
	connect(ui.searchDirAddButton, SIGNAL(clicked(bool)), this, SLOT(searchDirAddButtonClicked(bool)));
	connect(ui.searchFileEditButton, SIGNAL(clicked(bool)), this, SLOT(searchFileEditButtonClicked(bool)));
	connect(ui.searchFileRemoveButton, SIGNAL(clicked(bool)), this, SLOT(searchFileRemoveButtonClicked(bool)));
	connect(ui.searchFileList, SIGNAL(itemSelectionChanged()), this, SLOT(searchResourceSelectionChanged()));

	connect(ui.datFileAddButton, SIGNAL(clicked(bool)), this, SLOT(datFileAddButtonClicked(bool)));
	connect(ui.datFileEditButton, SIGNAL(clicked(bool)), this, SLOT(datFileEditButtonClicked(bool)));
	connect(ui.datFileRemoveButton, SIGNAL(clicked(bool)), this, SLOT(datFileRemoveButtonClicked(bool)));
	connect(ui.datFileList, SIGNAL(itemSelectionChanged()), this, SLOT(datSelectionChanged()));

	displayedProfile = profile;
	clearFiles();

	ui.nameField->setText(profile->getName());

	if (profile->getGameInfo())
		ui.rootDirField->setText(profile->getGameInfo()->getRootDirectory().getPath()->toString());
	else
		ui.rootDirField->setText("");

	if (profile->getGameInfo()) {
		switch (profile->getGameInfo()->getVersionMode()) {
		case GameInfo::GTASA:
			ui.versionBox->setCurrentIndex(0);
			break;
		case GameInfo::GTAVC:
			ui.versionBox->setCurrentIndex(1);
			break;
		case GameInfo::GTAIII:
			ui.versionBox->setCurrentIndex(2);
			break;
		}
	}

	Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		new QListWidgetItem((*it)->getPath()->toString(), ui.engineFileList);
	}

	for (it = profile->getSearchResourceBegin() ; it != profile->getSearchResourceEnd() ; it++) {
		new QListWidgetItem((*it)->getPath()->toString(), ui.searchFileList);
	}

	for (it = profile->getDATFilesBegin() ; it != profile->getDATFilesEnd() ; it++) {
		new QListWidgetItem((*it)->getPath()->toString(), ui.datFileList);
	}

	ui.engineFileAddButton->setEnabled(profile != NULL);
	ui.engineDirAddButton->setEnabled(profile != NULL);
	ui.engineFileEditButton->setEnabled(profile != NULL);
	ui.engineFileRemoveButton->setEnabled(profile != NULL);

	ui.searchFileAddButton->setEnabled(profile != NULL);
	ui.searchDirAddButton->setEnabled(profile != NULL);
	ui.searchFileEditButton->setEnabled(profile != NULL);
	ui.searchFileRemoveButton->setEnabled(profile != NULL);

	ui.datFileAddButton->setEnabled(profile != NULL);
	ui.datFileEditButton->setEnabled(profile != NULL);
	ui.datFileRemoveButton->setEnabled(profile != NULL);

	if (profile)
		tryAutocomplete();
}


ProfileConfigWidget::~ProfileConfigWidget()
{
	clearFiles();
}


void ProfileConfigWidget::getResourceFiles(QLinkedList<QString>& dest)
{
	for (int i = 0 ; i < ui.engineFileList->count() ; i++) {
		QListWidgetItem* item = ui.engineFileList->item(i);
		dest << item->text();
	}
}


void ProfileConfigWidget::getSearchResourceFiles(QLinkedList<QString>& dest)
{
	for (int i = 0 ; i < ui.searchFileList->count() ; i++) {
		QListWidgetItem* item = ui.searchFileList->item(i);
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
	int count = ui.engineFileList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.engineFileList->takeItem(0);
	}

	count = ui.searchFileList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.searchFileList->takeItem(0);
	}

	count = ui.datFileList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.datFileList->takeItem(0);
	}
}


void ProfileConfigWidget::versionBoxChanged(int index)
{

}


void ProfileConfigWidget::engineFileAddButtonClicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select one or more resources"));
	QStringList filesCpy = files;
	QStringList::iterator it;

	for (it = filesCpy.begin() ; it != filesCpy.end() ; it++) {
		new QListWidgetItem(*it, ui.engineFileList);
	}
}


void ProfileConfigWidget::engineDirAddButtonClicked(bool checked)
{
	QString fname = QFileDialog::getExistingDirectory(this, tr("Select a resource directory"));

	if (!fname.isNull()) {
		new QListWidgetItem(fname, ui.engineFileList);
	}
}


void ProfileConfigWidget::engineFileEditButtonClicked(bool checked)
{
	QListWidgetItem* item = ui.engineFileList->currentItem();

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


void ProfileConfigWidget::engineFileRemoveButtonClicked(bool checked)
{
	QList<QListWidgetItem*> items = ui.engineFileList->selectedItems();

	for (int i = 0 ; i < items.count() ; i++) {
		QListWidgetItem* item = items[i];
		delete item;
	}
}


void ProfileConfigWidget::engineResourceSelectionChanged()
{
	int numSel = ui.engineFileList->selectedItems().count();

	ui.engineFileRemoveButton->setEnabled(numSel > 0);
	ui.engineFileEditButton->setEnabled(numSel == 1);
}


void ProfileConfigWidget::searchFileAddButtonClicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select one or more resources"));
	QStringList filesCpy = files;
	QStringList::iterator it;

	for (it = filesCpy.begin() ; it != filesCpy.end() ; it++) {
		new QListWidgetItem(*it, ui.searchFileList);
	}
}


void ProfileConfigWidget::searchDirAddButtonClicked(bool checked)
{
	QString fname = QFileDialog::getExistingDirectory(this, tr("Select a resource directory"));

	if (!fname.isNull()) {
		new QListWidgetItem(fname, ui.searchFileList);
	}
}


void ProfileConfigWidget::searchFileEditButtonClicked(bool checked)
{
	QListWidgetItem* item = ui.searchFileList->currentItem();

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


void ProfileConfigWidget::searchFileRemoveButtonClicked(bool checked)
{
	QList<QListWidgetItem*> items = ui.searchFileList->selectedItems();

	for (int i = 0 ; i < items.count() ; i++) {
		QListWidgetItem* item = items[i];
		delete item;
	}
}


void ProfileConfigWidget::searchResourceSelectionChanged()
{
	int numSel = ui.searchFileList->selectedItems().count();

	ui.searchFileRemoveButton->setEnabled(numSel > 0);
	ui.searchFileEditButton->setEnabled(numSel == 1);
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


void ProfileConfigWidget::datSelectionChanged()
{
	int numSel = ui.datFileList->selectedItems().count();

	ui.datFileRemoveButton->setEnabled(numSel > 0);
	ui.datFileEditButton->setEnabled(numSel == 1);
}


void ProfileConfigWidget::rootDirButtonClicked(bool checked)
{
	QString dname = QFileDialog::getExistingDirectory(this, tr("Choose a DAT root directory"));

	if (!dname.isNull()) {
		ui.rootDirField->setText(dname);
	}

	tryAutocomplete();
}


void ProfileConfigWidget::tryAutocomplete()
{
	// Try to guess the game version
	bool verGuessed = false;
	GameInfo::VersionMode ver;

	File root(ui.rootDirField->text().toLocal8Bit().constData());

	if (root.exists()  &&  root.isDirectory()) {
		FileIterator* it = (root.getIterator());
		File* child;

		while ((child = it->next())  !=  NULL) {
			if (QString(child->getPath()->getFileName()).toLower() == QString("gta_sa.exe")) {
				ver = GameInfo::GTASA;
				verGuessed = true;
				delete child;
				break;
			}

			delete child;
		}

		delete it;
	}

	if (verGuessed) {
		switch (ver) {
		case GameInfo::GTASA:
			ui.versionBox->setCurrentIndex(0);
			break;
		case GameInfo::GTAVC:
			ui.versionBox->setCurrentIndex(0);
			break;
		case GameInfo::GTAIII:
			ui.versionBox->setCurrentIndex(0);
			break;
		}

		// Try to autocomplete the DAT files
		if (ui.datFileList->count() == 0) {
			if (ver == GameInfo::GTASA) {
				new QListWidgetItem(File(root, "data/default.dat").getPath()->toString(),
						ui.datFileList);
				new QListWidgetItem(File(root, "data/gta.dat").getPath()->toString(),
						ui.datFileList);
			}
		}
	}
}
