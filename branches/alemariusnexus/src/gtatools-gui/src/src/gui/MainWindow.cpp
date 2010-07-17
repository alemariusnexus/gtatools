/*
 * MainWindow.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "MainWindow.h"
#include "FileItemModel.h"
#include "../ProfileManager.h"
#include "../formats/FormatManager.h"
#include "../formats/FormatHandler.h"
#include <qboxlayout.h>
#include <qlist.h>
#include <qsettings.h>
#include "ConfigWidget.h"
#include <qmenu.h>
#include <config.h>
#include "../formats/txd/TXDGUIModule.h"
#include "../System.h"
#include "DefaultGUIModule.h"



MainWindow::MainWindow()
		: currentDisplayWidget(NULL)
{
	ui.setupUi(this);
	setWindowTitle(tr("ProgramBaseName"));
	ui.mainSplitter->setSizes(QList<int>() << width()/3 << width()/3*2);
}


void MainWindow::initialize()
{
	ProfileManager* pm = ProfileManager::getInstance();
	System* sys = System::getInstance();

	Profile* currentProfile = pm->getCurrentProfile();

	currentProfileChanged(NULL, currentProfile);

	connect(sys, SIGNAL(fileOpened(const File&)), this, SLOT(openFile(const File&)));
	connect(sys, SIGNAL(currentFileClosed()), this, SLOT(closeCurrentFile()));
	connect(ui.fileTree, SIGNAL(activated(const QModelIndex&)), this, SLOT(fileSelectedInTree(const QModelIndex&)));
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
	connect(ui.fileTree, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(fileTreeContextMenuRequested(const QPoint&)));

	globalModules << new DefaultGUIModule(this);
	globalModules << new TXDGUIModule(this);

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	resize(settings.value("gui/mainwindow_size", size()).toSize());
}


MainWindow::~MainWindow()
{
	QLinkedList<GUIModule*>::iterator it;
	for (it = globalModules.begin() ; it != globalModules.end() ; it++) {
		delete *it;
	}

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	settings.setValue("gui/mainwindow_size", size());
}


void MainWindow::fileSelectedInTree(const QModelIndex& index)
{
	if (!index.isValid()) {
		return;
	}

	File* file = static_cast<StaticFile*>(index.internalPointer())->getFile();
	System::getInstance()->openFile(*file);
}


void MainWindow::openFile(const File& file)
{
	FileItemModel* model = (FileItemModel*) ui.fileTree->model();
	if (*model->getFileForIndex(ui.fileTree->currentIndex()) != file) {
		QModelIndex index = indexOfFileInTree(file);

		if (index.isValid()) {
			ui.fileTree->setCurrentIndex(index);
		}
	}

	closeCurrentFile();

	ui.fileNameLabel->setText(QString(file.getPath()->getFileName()));

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

		QLinkedList<FormatHandler*> handlers = FormatManager::getInstance()->getHandlers(file);

		if (handlers.size() == 0) {
			ui.fileTypeLabel->setText(QString(tr("Unrecognized File")));
		} else {
			ui.fileTypeLabel->setText((*handlers.begin())->getFormatName(&file));
		}

		QLinkedList<FormatHandler*>::iterator it;

		for (it = handlers.begin() ; it != handlers.end() ; it++) {
			FormatHandler* handler = *it;
			GUIModule* mod = handler->createGUIModuleForFile(file, this);

			if (mod) {
				currentFileModules << mod;
			}
		}

		for (it = handlers.begin() ; it != handlers.end() ; it++) {
			FormatHandler* handler = *it;
			currentDisplayWidget = handler->createWidgetForFile(file, ui.contentPluginWidget);

			if (currentDisplayWidget) {
				ui.contentPluginWidget->layout()->addWidget(currentDisplayWidget);
				break;
			}
		}
	}
}


void MainWindow::closeCurrentFile()
{
	QLinkedList<GUIModule*>::iterator it;
	for (it = currentFileModules.begin() ; it != currentFileModules.end() ; it++) {
		delete *it;
	}
	currentFileModules.clear();

	if (currentDisplayWidget) {
		delete currentDisplayWidget;
		currentDisplayWidget = NULL;
	}

	ui.fileNameLabel->setText(tr("(No File Opened)"));
	ui.fileTypeLabel->setText("-");
	ui.fileSizeLabel->setText("-");
}


void MainWindow::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	FileItemModel* model = new FileItemModel(newProfile);

	setWindowTitle(QString(tr("GTATools GUI Tool - %1")).arg(newProfile->getName()));

	if (oldProfile != NULL) {
		FileItemModel* oldModel = (FileItemModel*) ui.fileTree->model();
		ui.fileTree->setModel(model);
		delete oldModel;

		disconnect(oldProfile, SIGNAL(changed()), this, SLOT(currentProfileContentChanged()));
	} else {
		ui.fileTree->setModel(model);
	}

	connect(newProfile, SIGNAL(changed()), this, SLOT(currentProfileContentChanged()));
}


void MainWindow::currentProfileContentChanged()
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	setWindowTitle(QString(tr("GTATools GUI Tool - %1")).arg(profile->getName()));

	FileItemModel* oldModel = (FileItemModel*) ui.fileTree->model();
	FileItemModel* model = new FileItemModel(profile);
	ui.fileTree->setModel(model);
	delete oldModel;
}


void MainWindow::fileTreeContextMenuRequested(const QPoint& pos)
{
	FileItemModel* model = (FileItemModel*) ui.fileTree->model();
	QModelIndex index = ui.fileTree->indexAt(pos);

	if (index.isValid()) {
		File* file = model->getFileForIndex(index);

		QMenu* menu = new QMenu(ui.fileTree);
		menu->setAttribute(Qt::WA_DeleteOnClose);

		QLinkedList<GUIModule*>::iterator it;
		for (it = globalModules.begin() ; it != globalModules.end() ; it++) {
			GUIModule* module = *it;
			module->buildFileTreeMenu(*file, *menu);
		}

		if (!file->isDirectory()) {
			QLinkedList<FormatHandler*> handlers = FormatManager::getInstance()->getHandlers(*file);
			QLinkedList<FormatHandler*>::iterator it;
			for (it = handlers.begin() ; it != handlers.end() ; it++) {
				FormatHandler* handler = *it;
				handler->buildFileTreeMenu(*file, *menu);
			}
		}

		for (it = currentFileModules.begin() ; it != currentFileModules.end() ; it++) {
			GUIModule* module = *it;
			module->buildFileTreeMenu(*file, *menu);
		}

		menu->popup(ui.fileTree->mapToGlobal(pos));
	}
}


QModelIndex MainWindow::indexOfFileInTree(const File& file)
{
	QModelIndex root = ui.fileTree->rootIndex();
	FileItemModel* model = (FileItemModel*) ui.fileTree->model();
	return model->indexOf(file, root);
}

