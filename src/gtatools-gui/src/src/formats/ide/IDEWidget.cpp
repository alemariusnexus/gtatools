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

#include "../../config.h"
#include "IDEWidget.h"
#include <gtaformats/ide/IDEReader.h>
#include <gtaformats/ide/IDEStaticObject.h>
#include <gtaformats/ide/IDETimedObject.h>
#include <gtaformats/ide/IDEAnimation.h>
#include <gtaformats/ide/IDEPedestrian.h>
#include <gtaformats/ide/IDEWeapon.h>
#include <gtaformats/util/DefaultFileFinder.h>
#include <QtCore/QRegExp>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QColor>
#include "../../System.h"
#include "../../ProfileManager.h"
#include "../../Profile.h"
#include "../../gui/GUI.h"



IDEWidget::IDEWidget(QWidget* parent, const FileOpenRequest& request)
		: QWidget(parent), linkBrush(QColor(Qt::blue))
{
	ui.setupUi(this);

	File* file = request.getFile();
	QVariant lineVariant = request.getAttribute("line");
	int selectedLine = lineVariant.isNull() ? -1 : lineVariant.toInt();

	QString content;

	istream* stream = file->openInputStream();

	char buffer[4096];
	while (!stream->eof()) {
		stream->read(buffer, sizeof(buffer)-1);
		buffer[stream->gcount()] = '\0';
		content.append(buffer);
	}

	delete stream;


	// Apply some basic syntax highlighting
	QString richContent("<pre>");
	QStringList lines = content.split('\n');
	QStringList::iterator it;

	QString keywords[] = {
			"end", "objs", "tobj", "anim", "peds", "weap", "cars", "hier", "txdp", "2dfx", "path"
	};

	for (it = lines.begin() ; it != lines.end() ; it++) {
		QString line = *it;
		QString cline = line;

		int cmtIdx = line.indexOf('#');

		if (cmtIdx != -1) {
			line.insert(cmtIdx, "<font color=\"#AAAAAA\">");
			line.append("</font>");
			cline = line.left(cmtIdx);
		}

		for (unsigned int i = 0 ; i < sizeof(keywords)/sizeof(keywords[0]) ; i++) {
			QString kw = keywords[i];
			QRegExp regex(QString("^(\\s*)(%1)(\\s*)$").arg(kw));
			cline.replace(regex, "\\1<font color=\"blue\"><b>\\2</b></font>\\3");
		}

		cline.replace(",", "<font color=\"red\">,</font>");

		if (cmtIdx != -1) {
			line.replace(0, cmtIdx, cline);
		} else {
			line = cline;
		}

		richContent.append(line).append('\n');
	}

	richContent.append("</pre>");

	ui.textEdit->setHtml(richContent);


	// Some kind of bug in QtDesigner incorrectly sets the visibility property of the headers to a rather
	// random value.
	ui.staticObjectTable->horizontalHeader()->setVisible(true);
	ui.timedObjectTable->horizontalHeader()->setVisible(true);
	ui.animationTable->horizontalHeader()->setVisible(true);
	ui.pedTable->horizontalHeader()->setVisible(true);
	ui.weaponTable->horizontalHeader()->setVisible(true);

	IDEReader ide(*file);

	IDEStatement* stmt;

	while ((stmt = ide.readStatement())  !=  NULL) {
		idetype_t type = stmt->getType();

		int line = ide.getLastReadLine();

		if (type == IDE_TYPE_STATIC_OBJECT) {
			IDEStaticObject* sobj = (IDEStaticObject*) stmt;
			int rc = ui.staticObjectTable->rowCount();
			ui.staticObjectTable->setRowCount(rc+1);
			ui.staticObjectTable->setItem(rc, 0, createItem(QString("%1").arg(sobj->getId())));
			ui.staticObjectTable->setItem(rc, 1, createItem(sobj->getModelName(), true));
			ui.staticObjectTable->setItem(rc, 2, createItem(sobj->getTextureName(), true));
			ui.staticObjectTable->setItem(rc, 3, createItem(QString("%1")
					.arg(sobj->getNumSubObjects())));
			ui.staticObjectTable->setItem(rc, 5, createItem(QString("%1")
					.arg(sobj->getFlags())));

			QStringList ddists;

			for (int32_t i = 0 ; i < sobj->getNumSubObjects() ; i++) {
				ddists << QString("%1").arg(sobj->getDrawDistances()[i]);
			}

			ui.staticObjectTable->setItem(rc, 4, createItem(ddists.join(", ")));

			if (line == selectedLine) {
				ui.staticObjectTable->setCurrentCell(rc, 0);
				ui.tabWidget->setCurrentWidget(ui.staticObjectWidget);
			}
		} else if (type == IDE_TYPE_TIMED_OBJECT) {
			IDETimedObject* tobj = (IDETimedObject*) stmt;
			int rc = ui.timedObjectTable->rowCount();
			ui.timedObjectTable->setRowCount(rc+1);
			ui.timedObjectTable->setItem(rc, 0, createItem(QString("%1").arg(tobj->getId())));
			ui.timedObjectTable->setItem(rc, 1, createItem(tobj->getModelName(), true));
			ui.timedObjectTable->setItem(rc, 2, createItem(tobj->getTextureName(), true));
			ui.timedObjectTable->setItem(rc, 3, createItem(QString("%1")
					.arg(tobj->getNumSubObjects())));
			ui.timedObjectTable->setItem(rc, 5, createItem(QString("%1")
					.arg(tobj->getFlags())));
			ui.timedObjectTable->setItem(rc, 6, createItem(QString("%1")
					.arg(tobj->getTimeOn())));
			ui.timedObjectTable->setItem(rc, 7, createItem(QString("%1")
					.arg(tobj->getTimeOff())));

			QStringList ddists;

			for (int32_t i = 0 ; i < tobj->getNumSubObjects() ; i++) {
				ddists << QString("%1").arg(tobj->getDrawDistances()[i]);
			}

			ui.timedObjectTable->setItem(rc, 4, createItem(ddists.join(", ")));

			if (line == selectedLine) {
				ui.timedObjectTable->setCurrentCell(rc, 0);
				ui.tabWidget->setCurrentWidget(ui.timedObjectWidget);
			}
		} else if (type == IDE_TYPE_ANIMATION) {
			IDEAnimation* anim = (IDEAnimation*) stmt;
			int rc = ui.animationTable->rowCount();
			ui.animationTable->setRowCount(rc+1);
			ui.animationTable->setItem(rc, 0, createItem(QString("%1").arg(anim->getId())));
			ui.animationTable->setItem(rc, 1, createItem(anim->getModelName(), true));
			ui.animationTable->setItem(rc, 2, createItem(anim->getTextureName(), true));
			ui.animationTable->setItem(rc, 3, createItem(anim->getAnimationName()));
			ui.animationTable->setItem(rc, 4, createItem(QString("%1")
					.arg(anim->getDrawDist())));
			ui.animationTable->setItem(rc, 5, createItem(QString("%1").arg(anim->getFlags())));

			if (line == selectedLine) {
				ui.animationTable->setCurrentCell(rc, 0);
				ui.tabWidget->setCurrentWidget(ui.animationWidget);
			}
		} else if (type == IDE_TYPE_PEDESTRIAN) {
			IDEPedestrian* ped = (IDEPedestrian*) stmt;
			int rc = ui.pedTable->rowCount();
			ui.pedTable->setRowCount(rc+1);
			ui.pedTable->setItem(rc, 0, createItem(QString("%1").arg(ped->getId())));
			ui.pedTable->setItem(rc, 1, createItem(ped->getModelName(), true));
			ui.pedTable->setItem(rc, 2, createItem(ped->getTXDName(), true));
			ui.pedTable->setItem(rc, 3, createItem(ped->getDefaultPedType()));
			ui.pedTable->setItem(rc, 4, createItem(ped->getBehavior()));
			ui.pedTable->setItem(rc, 5, createItem(ped->getAnimationGroup()));
			ui.pedTable->setItem(rc, 6, createItem(QString("%1")
					.arg(ped->getDrivableCarClasses(), 0, 16)));
			ui.pedTable->setItem(rc, 7, createItem(QString("%1").arg(ped->getFlags())));
			ui.pedTable->setItem(rc, 8, createItem(ped->getSecondaryAnimationFile()));
			ui.pedTable->setItem(rc, 9, createItem(QString("%1")
					.arg(ped->getPreferredRadio1())));
			ui.pedTable->setItem(rc, 10, createItem(QString("%1")
					.arg(ped->getPreferredRadio2())));
			ui.pedTable->setItem(rc, 11, createItem(ped->getVoiceFile()));
			ui.pedTable->setItem(rc, 12, createItem(ped->getVoice1()));
			ui.pedTable->setItem(rc, 13, createItem(ped->getVoice2()));

			if (line == selectedLine) {
				ui.pedTable->setCurrentCell(rc, 0);
				ui.tabWidget->setCurrentWidget(ui.pedWidget);
			}
		} else if (type == IDE_TYPE_WEAPON) {
			IDEWeapon* weap = (IDEWeapon*) stmt;
			int rc = ui.weaponTable->rowCount();
			ui.weaponTable->setRowCount(rc+1);
			ui.weaponTable->setItem(rc, 0, createItem(QString("%1").arg(weap->getId())));
			ui.weaponTable->setItem(rc, 1, createItem(weap->getModelName(), true));
			ui.weaponTable->setItem(rc, 2, createItem(weap->getTXDName(), true));
			ui.weaponTable->setItem(rc, 3, createItem(weap->getAnimationName()));
			ui.weaponTable->setItem(rc, 4, createItem(QString("%1")
					.arg(weap->getObjectCount())));
			ui.weaponTable->setItem(rc, 6, createItem(QString("%1").arg(weap->getFlags())));

			QStringList ddists;

			for (int32_t i = 0 ; i < weap->getObjectCount() ; i++) {
				ddists << QString("%1").arg(weap->getDrawDistances()[i]);
			}

			ui.weaponTable->setItem(rc, 5, createItem(ddists.join(", ")));

			if (line == selectedLine) {
				ui.weaponTable->setCurrentCell(rc, 0);
				ui.tabWidget->setCurrentWidget(ui.weaponWidget);
			}
		}

		delete stmt;
	}

	ErrorLog* log = ide.getErrorLog();

	if (log->getMessageCount() > 0) {
		System* sys = System::getInstance();
		char* elem;

		sys->logError(tr("Parsing errors in IDE file %1:").arg(file->getPath()->getFileName()));

		while ((elem = log->nextMessage())  !=  NULL) {
			sys->logError(QString("\t%1").arg(elem));
			delete[] elem;
		}
	}

	ui.staticObjectTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui.timedObjectTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui.animationTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui.pedTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui.weaponTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

	connect(ui.staticObjectTable, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(staticObjectTableCellDoubleClicked(int, int)));
	connect(ui.timedObjectTable, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(timedObjectTableCellDoubleClicked(int, int)));
	connect(ui.animationTable, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(animationTableCellDoubleClicked(int, int)));
	connect(ui.pedTable, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(pedTableCellDoubleClicked(int, int)));
	connect(ui.weaponTable, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(weaponTableCellDoubleClicked(int, int)));
}


QTableWidgetItem* IDEWidget::createItem(const QString& text, bool link)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	if (link) {
		item->setForeground(linkBrush);
		item->setToolTip(tr("Double click to open file"));
	}

	return item;
}


void IDEWidget::staticObjectTableCellDoubleClicked(int row, int col)
{
	QTableWidgetItem* item = ui.staticObjectTable->item(row, col);

	if (col == 1) {
		// Model Name
		QString fname = item->text();
		fname.append(".dff");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	} else if (col == 2) {
		// TXD Name
		QString fname = item->text();
		fname.append(".txd");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	}
}


void IDEWidget::timedObjectTableCellDoubleClicked(int row, int col)
{
	QTableWidgetItem* item = ui.timedObjectTable->item(row, col);

	if (col == 1) {
		// Model Name
		QString fname = item->text();
		fname.append(".dff");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	} else if (col == 2) {
		// TXD Name
		QString fname = item->text();
		fname.append(".txd");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	}
}


void IDEWidget::animationTableCellDoubleClicked(int row, int col)
{
	QTableWidgetItem* item = ui.animationTable->item(row, col);

	if (col == 1) {
		// Model Name
		QString fname = item->text();
		fname.append(".dff");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	} else if (col == 2) {
		// TXD Name
		QString fname = item->text();
		fname.append(".txd");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	}
}


void IDEWidget::pedTableCellDoubleClicked(int row, int col)
{
	QTableWidgetItem* item = ui.pedTable->item(row, col);

	if (col == 1) {
		// Model Name
		QString fname = item->text();
		fname.append(".dff");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	} else if (col == 2) {
		// TXD Name
		QString fname = item->text();
		fname.append(".txd");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	}
}


void IDEWidget::weaponTableCellDoubleClicked(int row, int col)
{
	QTableWidgetItem* item = ui.weaponTable->item(row, col);

	if (col == 1) {
		// Model Name
		QString fname = item->text();
		fname.append(".dff");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	} else if (col == 2) {
		// TXD Name
		QString fname = item->text();
		fname.append(".txd");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	}
}
