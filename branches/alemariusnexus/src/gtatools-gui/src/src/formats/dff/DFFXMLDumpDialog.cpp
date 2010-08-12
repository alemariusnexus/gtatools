/*
 * DFFXMLDumpDialog.cpp
 *
 *  Created on: 07.08.2010
 *      Author: alemariusnexus
 */

#include "DFFXMLDumpDialog.h"


DFFXMLDumpDialog::DFFXMLDumpDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

