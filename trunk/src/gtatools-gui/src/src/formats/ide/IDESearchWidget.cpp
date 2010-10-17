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

#include "IDESearchWidget.h"
#include "IDEFileFinder.h"
#include "../../gui/GUI.h"
#include "../../System.h"




IDESearchWidget::IDESearchWidget(QWidget* parent)
		: QDialog(parent), finder(NULL)
{
	ui.setupUi(this);

	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(onSearch(bool)));
}


void IDESearchWidget::onCancel(bool checked)
{
	if (finder) {
		finder->interrupt();
	}

	close();
}


void IDESearchWidget::onSearch(bool checked)
{
	ui.searchButton->setEnabled(false);

	int32_t id = ui.idSpinner->value();
	finder = new IDEFileFinder(id);
	File* toBeOpened = GUI::getInstance()->findFile(finder, this);

	if (toBeOpened) {
		int line = finder->getMatchedLine(*toBeOpened);
		FileOpenRequest req(*toBeOpened);
		req.setAttribute("line", line);
		System::getInstance()->openFile(req);
	}

	delete toBeOpened;
	delete finder;
	finder = NULL;

	if (toBeOpened) {
		close();
	} else {
		ui.searchButton->setEnabled(true);
	}
}
