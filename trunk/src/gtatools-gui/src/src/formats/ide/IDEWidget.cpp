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
#include <QtCore/QRegExp>
#include <QtGui/QMessageBox>
#include "../../System.h"



IDEWidget::IDEWidget(QWidget* parent, const File& file)
		: QWidget(parent)
{
	ui.setupUi(this);

	QString content;

	InputStream* stream = file.openStream();

	char buffer[4096];
	while (!stream->hasReachedEnd()) {
		stream->read(buffer, sizeof(buffer)-1);
		buffer[stream->getLastReadCount()] = '\0';
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

	IDEReader ide(file);

	IDEStatement* stmt;

	while ((stmt = ide.readStatement())  !=  NULL) {
		idetype_t type = stmt->getType();

		if (type == IDE_TYPE_STATIC_OBJECT) {
			IDEStaticObject* sobj = (IDEStaticObject*) stmt;
			int rc = ui.staticObjectTable->rowCount();
			ui.staticObjectTable->setRowCount(rc+1);
			ui.staticObjectTable->setItem(rc, 0, new QTableWidgetItem(QString("%1").arg(sobj->getId())));
			ui.staticObjectTable->setItem(rc, 1, new QTableWidgetItem(sobj->getModelName()));
			ui.staticObjectTable->setItem(rc, 2, new QTableWidgetItem(sobj->getTextureName()));
			ui.staticObjectTable->setItem(rc, 3, new QTableWidgetItem(QString("%1")
					.arg(sobj->getNumSubObjects())));
			ui.staticObjectTable->setItem(rc, 5, new QTableWidgetItem(QString("%1")
					.arg(sobj->getFlags())));

			QStringList ddists;

			for (int32_t i = 0 ; i < sobj->getNumSubObjects() ; i++) {
				ddists << QString("%1").arg(sobj->getDrawDistances()[i]);
			}

			ui.staticObjectTable->setItem(rc, 4, new QTableWidgetItem(ddists.join(", ")));
		} else if (type == IDE_TYPE_TIMED_OBJECT) {
			IDETimedObject* tobj = (IDETimedObject*) stmt;
			int rc = ui.timedObjectTable->rowCount();
			ui.timedObjectTable->setRowCount(rc+1);
			ui.timedObjectTable->setItem(rc, 0, new QTableWidgetItem(QString("%1").arg(tobj->getId())));
			ui.timedObjectTable->setItem(rc, 1, new QTableWidgetItem(tobj->getModelName()));
			ui.timedObjectTable->setItem(rc, 2, new QTableWidgetItem(tobj->getTextureName()));
			ui.timedObjectTable->setItem(rc, 3, new QTableWidgetItem(QString("%1")
					.arg(tobj->getNumSubObjects())));
			ui.timedObjectTable->setItem(rc, 5, new QTableWidgetItem(QString("%1")
					.arg(tobj->getFlags())));
			ui.timedObjectTable->setItem(rc, 6, new QTableWidgetItem(QString("%1")
					.arg(tobj->getTimeOn())));
			ui.timedObjectTable->setItem(rc, 7, new QTableWidgetItem(QString("%1")
					.arg(tobj->getTimeOff())));

			QStringList ddists;

			for (int32_t i = 0 ; i < tobj->getNumSubObjects() ; i++) {
				ddists << QString("%1").arg(tobj->getDrawDistances()[i]);
			}

			ui.timedObjectTable->setItem(rc, 4, new QTableWidgetItem(ddists.join(", ")));
		} else if (type == IDE_TYPE_ANIMATION) {
			IDEAnimation* anim = (IDEAnimation*) stmt;
			int rc = ui.animationTable->rowCount();
			ui.animationTable->setRowCount(rc+1);
			ui.animationTable->setItem(rc, 0, new QTableWidgetItem(QString("%1").arg(anim->getId())));
			ui.animationTable->setItem(rc, 1, new QTableWidgetItem(anim->getModelName()));
			ui.animationTable->setItem(rc, 2, new QTableWidgetItem(anim->getTextureName()));
			ui.animationTable->setItem(rc, 3, new QTableWidgetItem(anim->getAnimationName()));
			ui.animationTable->setItem(rc, 4, new QTableWidgetItem(QString("%1")
					.arg(anim->getDrawDist())));
			ui.animationTable->setItem(rc, 5, new QTableWidgetItem(QString("%1").arg(anim->getFlags())));
		} else if (type == IDE_TYPE_PEDESTRIAN) {
			IDEPedestrian* ped = (IDEPedestrian*) stmt;
			int rc = ui.pedTable->rowCount();
			ui.pedTable->setRowCount(rc+1);
			ui.pedTable->setItem(rc, 0, new QTableWidgetItem(QString("%1").arg(ped->getId())));
			ui.pedTable->setItem(rc, 1, new QTableWidgetItem(ped->getModelName()));
			ui.pedTable->setItem(rc, 2, new QTableWidgetItem(ped->getTXDName()));
			ui.pedTable->setItem(rc, 3, new QTableWidgetItem(ped->getDefaultPedType()));
			ui.pedTable->setItem(rc, 4, new QTableWidgetItem(ped->getBehavior()));
			ui.pedTable->setItem(rc, 5, new QTableWidgetItem(ped->getAnimationGroup()));
			ui.pedTable->setItem(rc, 6, new QTableWidgetItem(QString("%1")
					.arg(ped->getDrivableCarClasses(), 0, 16)));
			ui.pedTable->setItem(rc, 7, new QTableWidgetItem(QString("%1").arg(ped->getFlags())));
			ui.pedTable->setItem(rc, 8, new QTableWidgetItem(ped->getSecondaryAnimationFile()));
			ui.pedTable->setItem(rc, 9, new QTableWidgetItem(QString("%1")
					.arg(ped->getPreferredRadio1())));
			ui.pedTable->setItem(rc, 10, new QTableWidgetItem(QString("%1")
					.arg(ped->getPreferredRadio2())));
			ui.pedTable->setItem(rc, 11, new QTableWidgetItem(ped->getVoiceFile()));
			ui.pedTable->setItem(rc, 12, new QTableWidgetItem(ped->getVoice1()));
			ui.pedTable->setItem(rc, 13, new QTableWidgetItem(ped->getVoice2()));
		} else if (type == IDE_TYPE_WEAPON) {
			IDEWeapon* weap = (IDEWeapon*) stmt;
			int rc = ui.weaponTable->rowCount();
			ui.weaponTable->setRowCount(rc+1);
			ui.weaponTable->setItem(rc, 0, new QTableWidgetItem(QString("%1").arg(weap->getId())));
			ui.weaponTable->setItem(rc, 1, new QTableWidgetItem(weap->getModelName()));
			ui.weaponTable->setItem(rc, 2, new QTableWidgetItem(weap->getTXDName()));
			ui.weaponTable->setItem(rc, 3, new QTableWidgetItem(weap->getAnimationName()));
			ui.weaponTable->setItem(rc, 4, new QTableWidgetItem(QString("%1")
					.arg(weap->getObjectCount())));
			ui.weaponTable->setItem(rc, 6, new QTableWidgetItem(QString("%1").arg(weap->getFlags())));

			QStringList ddists;

			for (int32_t i = 0 ; i < weap->getObjectCount() ; i++) {
				ddists << QString("%1").arg(weap->getDrawDistances()[i]);
			}

			ui.weaponTable->setItem(rc, 5, new QTableWidgetItem(ddists.join(", ")));
		}

		delete stmt;
	}

	ErrorLog* log = ide.getErrorLog();

	if (log->getMessageCount() > 0) {
		System* sys = System::getInstance();
		char* elem;

		sys->logError(tr("Parsing errors in IDE file %1:").arg(file.getPath()->getFileName()));

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
}
