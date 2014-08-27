/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "TestWindow.h"
#include <QtCore/QFile>
#include <QBoxLayout>
#include <QAction>



TestWindow::TestWindow()
		: QMainWindow()
{
	QWidget* cw = new QWidget(this);

	QVBoxLayout* layout = new QVBoxLayout(cw);
	cw->setLayout(layout);

	setCentralWidget(cw);

	textEdit = new QTextEdit(this);
	textEdit->setReadOnly(true);

	QFile file("/home/alemariusnexus/test.htm");
	file.open(QFile::ReadOnly);
	QByteArray bcont = file.readAll();
	QString cont(bcont);
	textEdit->setHtml(bcont);

	printf("%u bytes!\n", bcont.size());

	layout->addWidget(textEdit);
}

