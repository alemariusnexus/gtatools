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
#include <qprogressbar.h>
#include <qheaderview.h>
#include <qlayout.h>
#include <qtablewidget.h>



MainWindow::MainWindow()
		: currentDisplayWidget(NULL), hasOpenFile(false)
{
	ui.setupUi(this);
	setWindowTitle(tr("ProgramBaseName"));
	ui.mainSplitter->setSizes(QList<int>() << width()/3 << width()/3*2);
	progressBar = new QProgressBar(ui.statusbar);
	progressBar->setMaximumWidth(400);
	ui.statusbar->addPermanentWidget(progressBar);

	System* sys = System::getInstance();
	sys->setMainWindow(this);

	connect(sys, SIGNAL(taskStarted(int, int, const QString&)), this, SLOT(taskStarted(int, int, const QString&)));
	connect(sys, SIGNAL(taskValueUpdated(int)), this, SLOT(taskValueUpdated(int)));
	connect(sys, SIGNAL(taskEnded()), this, SLOT(taskEnded()));
	connect(sys, SIGNAL(statusMessageShown(const QString&, int)), this, SLOT(statusMessageShown(const QString&, int)));
}


void MainWindow::initialize()
{
	ProfileManager* pm = ProfileManager::getInstance();
	System* sys = System::getInstance();

	Profile* currentProfile = pm->getCurrentProfile();

	currentProfileChanged(NULL, currentProfile);

	connect(sys, SIGNAL(fileOpened(const File&, const QHash<QString, QVariant>&)), this,
			SLOT(openFile(const File&, const QHash<QString, QVariant>&)));
	connect(sys, SIGNAL(currentFileClosed()), this, SLOT(closeCurrentFile()));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(configurationChanged()));
	connect(ui.fileTree, SIGNAL(activated(const QModelIndex&)), this, SLOT(fileSelectedInTree(const QModelIndex&)));
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
	connect(ui.fileTree, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(fileTreeContextMenuRequested(const QPoint&)));

	sys->installGUIModule(new DefaultGUIModule);
	sys->installGUIModule(new TXDGUIModule);

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	resize(settings.value("gui/mainwindow_size", size()).toSize());

	contentTabCompact = NULL;

	loadConfigUiSettings();
}


void MainWindow::loadConfigUiSettings()
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	if (settings.value("gui/compact_mode", false).toBool()) {
		if (!contentTabCompact) {
			QLayout* layout = ui.contentWidget->layout();
			layout->removeWidget(ui.fileInfoWidget);
			layout->removeWidget(ui.contentPluginWidget);

			QTabWidget* tw = new QTabWidget(ui.contentWidget);
			tw->addTab(ui.fileInfoWidget, tr("&General Information"));
			tw->addTab(ui.contentPluginWidget, tr("&Content"));
			tw->setTabEnabled(1, hasOpenFile);
			layout->addWidget(tw);

			contentTabCompact = tw;
		}
	} else {
		if (contentTabCompact) {
			QVBoxLayout* layout = (QVBoxLayout*) ui.contentWidget->layout();
			layout->removeWidget(contentTabCompact);
			contentTabCompact->removeTab(0);
			contentTabCompact->removeTab(1);
			ui.fileInfoWidget->setParent(ui.contentWidget);
			ui.contentPluginWidget->setParent(ui.contentWidget);
			ui.fileInfoWidget->show();
			ui.contentPluginWidget->show();
			delete contentTabCompact;
			layout->addWidget(ui.fileInfoWidget);
			layout->addWidget(ui.contentPluginWidget);
		}

		contentTabCompact = NULL;
	}

	ui.fileTree->header()->setResizeMode(0, QHeaderView::Stretch);
	ui.fileTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
}


MainWindow::~MainWindow()
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	settings.setValue("gui/mainwindow_size", size());

	if (currentDisplayWidget) {
		delete currentDisplayWidget;
	}

	System* sys = System::getInstance();
	QLinkedList<GUIModule*> modules = sys->getInstalledGUIModules();
	QLinkedList<GUIModule*>::iterator it;

	for (it = modules.begin() ; it != modules.end() ; it++) {
		GUIModule* module = *it;
		sys->uninstallGUIModule(module);
		delete module;
	}
}


void MainWindow::taskStarted(int min, int max, const QString& message)
{
	progressBar->setMinimum(min);
	progressBar->setMaximum(max);
	ui.statusbar->showMessage(message);
}


void MainWindow::taskValueUpdated(int value)
{
	progressBar->setValue(value);
}


void MainWindow::taskEnded()
{
	progressBar->reset();
	progressBar->update();
	ui.statusbar->clearMessage();
	System::getInstance()->showStatusMessage(tr("Done!"), 2000);
}


void MainWindow::statusMessageShown(const QString& message, int timeout)
{
	ui.statusbar->showMessage(message, timeout);
}


void MainWindow::fileSelectedInTree(const QModelIndex& index)
{
	if (!index.isValid()) {
		return;
	}

	File* file = static_cast<StaticFile*>(index.internalPointer())->getFile();
	System::getInstance()->openFile(*file);
}


void MainWindow::openFile(const File& file, const QHash<QString, QVariant>& data)
{
	FileItemModel* model = (FileItemModel*) ui.fileTree->model();
	QModelIndex current = ui.fileTree->currentIndex();

	if (!current.isValid()  ||  *model->getFileForIndex(ui.fileTree->currentIndex()) != file) {
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

		/*for (it = handlers.begin() ; it != handlers.end() ; it++) {
			FormatHandler* handler = *it;
			GUIModule* mod = handler->createGUIModuleForFile(file, this);

			if (mod) {
				currentFileModules << mod;
			}
		}*/

		for (it = handlers.begin() ; it != handlers.end() ; it++) {
			FormatHandler* handler = *it;
			currentDisplayWidget = handler->createWidgetForFile(file, ui.contentPluginWidget, data);

			if (currentDisplayWidget) {
				ui.contentPluginWidget->layout()->addWidget(currentDisplayWidget);
				break;
			}
		}

		if (contentTabCompact) {
			contentTabCompact->setTabEnabled(1, currentDisplayWidget != NULL);
		}
	}

	hasOpenFile = true;
}


void MainWindow::closeCurrentFile()
{
	/*QLinkedList<GUIModule*>::iterator it;
	for (it = currentFileModules.begin() ; it != currentFileModules.end() ; it++) {
		delete *it;
	}
	currentFileModules.clear();*/

	if (hasOpenFile) {
		if (currentDisplayWidget) {
			delete currentDisplayWidget;
			currentDisplayWidget = NULL;
		}

		ui.fileNameLabel->setText(tr("(No File Opened)"));
		ui.fileTypeLabel->setText("-");
		ui.fileSizeLabel->setText("-");

		hasOpenFile = false;
	}
}


void MainWindow::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	FileItemModel* model = new FileItemModel(newProfile, this);

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
	System* sys = System::getInstance();

	FileItemModel* model = (FileItemModel*) ui.fileTree->model();
	QModelIndex index = ui.fileTree->indexAt(pos);

	if (index.isValid()) {
		File* file = model->getFileForIndex(index);

		QMenu* menu = new QMenu(ui.fileTree);
		menu->setAttribute(Qt::WA_DeleteOnClose);

		QLinkedList<GUIModule*> modules = sys->getInstalledGUIModules();
		QLinkedList<GUIModule*>::iterator it;
		for (it = modules.begin() ; it != modules.end() ; it++) {
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

		menu->popup(ui.fileTree->mapToGlobal(pos));
	}
}


QModelIndex MainWindow::indexOfFileInTree(const File& file)
{
	QModelIndex root = ui.fileTree->rootIndex();
	FileItemModel* model = (FileItemModel*) ui.fileTree->model();
	return model->indexOf(file, root);
}


void MainWindow::configurationChanged()
{
	loadConfigUiSettings();
}

