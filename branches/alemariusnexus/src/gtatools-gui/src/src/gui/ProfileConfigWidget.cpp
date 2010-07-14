/*
 * ProfileConfigWidget.cpp
 *
 *  Created on: 12.07.2010
 *      Author: alemariusnexus
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
	QString fname = QFileDialog::getOpenFileName(this, tr("Select resource"));

	if (!fname.isNull()) {
		//ui.fileList->addItem(fname);
		new QListWidgetItem(fname, ui.fileList);
	}
}


void ProfileConfigWidget::dirAddButtonClicked(bool checked)
{
	QString fname = QFileDialog::getExistingDirectory(this, tr("Select resource"));

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
		QString newFname = QFileDialog::getExistingDirectory(this, tr("Select new resource"), fname);

		if (!newFname.isNull()) {
			item->setText(newFname);
		}
	} else {
		QString newFname = QFileDialog::getOpenFileName(this, tr("Select new resource"), fname);

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
