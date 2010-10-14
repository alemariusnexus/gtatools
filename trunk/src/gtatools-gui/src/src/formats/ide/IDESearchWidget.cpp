/*
 * IDESearchWidget.cpp
 *
 *  Created on: 15.10.2010
 *      Author: alemariusnexus
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
