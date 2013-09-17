/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "IPLWidget.h"
#include <gtaformats/ipl/IPLReader.h>
#include <gtaformats/ipl/IPLInstance.h>
#include <gtaformats/ipl/IPLCar.h>
#include <gtaformats/util/DefaultFileFinder.h>
#include "../../System.h"
#include "../../gui/GUI.h"



IPLWidget::IPLWidget(QWidget* parent, const File& file)
		: QWidget(parent), linkBrush(Qt::blue)
{
	ui.setupUi(this);

	QString content;

	IPLReader ipl(file);

	if (ipl.isBinary()) {
		QString instCode, carCode;

		IPLStatement* stmt;

		while ((stmt = ipl.readStatement())  !=  NULL) {
			ipltype_t type = stmt->getType();

			if (type == IPL_TYPE_INSTANCE) {
				IPLInstance* inst = (IPLInstance*) stmt;
				float x, y, z;
				float rx, ry, rz, rw;
				inst->getPosition(x, y, z);
				inst->getRotation(rx, ry, rz, rw);

				instCode.append(QString("%1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11\n")
						.arg(inst->getID()).arg("UNKNOWN").arg(inst->getInterior()).arg(x).arg(y).arg(z)
						.arg(rx).arg(ry).arg(rz).arg(rw).arg(inst->getLOD()));
			} else if (type == IPL_TYPE_INSTANCE) {
				IPLCar* car = (IPLCar*) stmt;
				float x, y, z;
				car->getPosition(x, y, z);

				carCode.append(QString("%1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12\n")
						.arg(x).arg(y).arg(z).arg(car->getAngle()).arg(car->getCarID())
						.arg(car->getPrimaryColor()).arg(car->getSecondaryColor()).arg(car->isForceSpawn())
						.arg(car->getAlarmProbability()).arg(car->getDoorLockProbability())
						.arg(car->getUnknown1()).arg(car->getUnknown2()));
			}
		}

		content.append("# Automatically decompiled from SA binary IPL format.\n\n");
		content.append("inst\n");
		content.append(instCode);
		content.append("end\n");
		content.append("cars\n");
		content.append(carCode);
		content.append("end\n");
	} else {
		istream* stream = file.openInputStream(istream::binary);

		char buffer[4096];
		while (!stream->eof()) {
			stream->read(buffer, sizeof(buffer)-1);
			buffer[stream->gcount()] = '\0';
			content.append(buffer);
		}

		delete stream;
	}


	// Apply some basic syntax highlighting
	QString richContent("<pre>");
	QStringList lines = content.split('\n');
	QStringList::iterator it;

	QString keywords[] = {
			"inst", "cull", "path", "grge", "enex", "pick", "jump", "tcyc", "auzo", "mult", "cars", "occl",
			"zone", "end"
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
	ui.instTable->horizontalHeader()->setVisible(true);
	ui.instTable->verticalHeader()->setVisible(true);
	ui.carTable->horizontalHeader()->setVisible(true);

	IPLReader ipl2(file);
	IPLStatement* stmt;

	while ((stmt = ipl2.readStatement())  !=  NULL) {
		ipltype_t type = stmt->getType();

		if (type == IPL_TYPE_INSTANCE) {
			IPLInstance* inst = (IPLInstance*) stmt;
			float x, y, z;
			float rx, ry, rz, rw;
			float sx, sy, sz;
			inst->getPosition(x, y, z);
			inst->getRotation(rx, ry, rz, rw);
			inst->getScale(sx, sy, sz);

			int rc = ui.instTable->rowCount();
			ui.instTable->setRowCount(rc+1);

			ui.instTable->setVerticalHeaderItem(rc, createItem(QString("%1").arg(rc)));
			ui.instTable->setItem(rc, 0, createItem(QString("%1").arg(inst->getID()), true));
			ui.instTable->setItem(rc, 1, createItem(inst->getModelName().get(), true));
			ui.instTable->setItem(rc, 2, createItem(QString("%1").arg(inst->getInterior())));
			ui.instTable->setItem(rc, 3, createItem(QString("%1").arg(x)));
			ui.instTable->setItem(rc, 4, createItem(QString("%1").arg(y)));
			ui.instTable->setItem(rc, 5, createItem(QString("%1").arg(z)));
			ui.instTable->setItem(rc, 6, createItem(QString("%1").arg(sx)));
			ui.instTable->setItem(rc, 7, createItem(QString("%1").arg(sy)));
			ui.instTable->setItem(rc, 8, createItem(QString("%1").arg(sz)));
			ui.instTable->setItem(rc, 9, createItem(QString("%1").arg(rx)));
			ui.instTable->setItem(rc, 10, createItem(QString("%1").arg(ry)));
			ui.instTable->setItem(rc, 11, createItem(QString("%1").arg(rz)));
			ui.instTable->setItem(rc, 12, createItem(QString("%1").arg(rw)));
			ui.instTable->setItem(rc, 13, createItem(QString("%1").arg(inst->getLOD())));
		} else if (type == IPL_TYPE_CAR) {
			IPLCar* car = (IPLCar*) stmt;
			float x, y, z;
			car->getPosition(x, y, z);

			int rc = ui.carTable->rowCount();
			ui.carTable->setRowCount(rc+1);

			ui.carTable->setItem(rc, 0, createItem(QString("%1").arg(x)));
			ui.carTable->setItem(rc, 1, createItem(QString("%1").arg(y)));
			ui.carTable->setItem(rc, 2, createItem(QString("%1").arg(z)));
			ui.carTable->setItem(rc, 3, createItem(QString("%1").arg(car->getAngle())));
			ui.carTable->setItem(rc, 4, createItem(QString("%1").arg(car->getCarID())));
			ui.carTable->setItem(rc, 5, createItem(QString("%1").arg(car->getPrimaryColor())));
			ui.carTable->setItem(rc, 6, createItem(QString("%1").arg(car->getSecondaryColor())));
			ui.carTable->setItem(rc, 7, createItem(car->isForceSpawn() ? "true" : "false"));
			ui.carTable->setItem(rc, 8, createItem(QString("%1").arg(car->getAlarmProbability())));
			ui.carTable->setItem(rc, 9, createItem(QString("%1").arg(car->getDoorLockProbability())));
			ui.carTable->setItem(rc, 10, createItem(QString("%1").arg(car->getUnknown1())));
			ui.carTable->setItem(rc, 11, createItem(QString("%1").arg(car->getUnknown2())));
		}

		delete stmt;
	}

	ErrorLog* log = ipl2.getErrorLog();

	if (log->getMessageCount() > 0) {
		System* sys = System::getInstance();
		char* elem;

		sys->log(LogEntry::error(tr("Parsing errors in IPL file %1:")
				.arg(file.getPath()->getFileName().get())));

		while ((elem = log->nextMessage())  !=  NULL) {
			sys->log(LogEntry::error(QString("\t%1").arg(elem)));
			delete[] elem;
		}
	}

	ui.instTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui.carTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

	connect(ui.instTable, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(instanceTableCellDoubleClicked(int, int)));
}


QTableWidgetItem* IPLWidget::createItem(const QString& text, bool link)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	if (link) {
		item->setForeground(linkBrush);
		item->setToolTip(tr("Double click to open file"));
	}

	return item;
}


void IPLWidget::instanceTableCellDoubleClicked(int row, int col)
{
	QTableWidgetItem* item = ui.instTable->item(row, col);

	if (col == 1) {
		QString fname(item->text());
		fname.append(".dff");
		DefaultFileFinder finder(fname.toLocal8Bit().constData(), false);
		GUI::getInstance()->findAndOpenFile(&finder, this);
	} else if (col == 0) {
		int id = item->text().toInt();
		SystemQuery query("FindItemDefinition");
		query["id"] = id;
		SystemQueryResult result = System::getInstance()->sendSystemQuery(query);

		if (result.isSuccessful()) {
			File file(result["file"].toString().toAscii().constData());
			int line = result["line"].toInt();
			EntityOpenRequest req;
			req.setAttribute("file", QString(file.getPath()->toString().get()));
			req.setAttribute("line", line);
			System::getInstance()->openEntity(req);
		}
	}
}
