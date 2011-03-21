/*
 * PVSDialog.h
 *
 *  Created on: 21.03.2011
 *      Author: alemariusnexus
 */

#ifndef PVSDIALOG_H_
#define PVSDIALOG_H_

#include <QtGui/QDialog>
#include <ui_PVSDialog.h>
#include <gtaformats/util/ProgressObserver.h>
#include "../Task.h"


class PVSDialog : public QDialog, public ProgressObserver
{
	Q_OBJECT

public:
	PVSDialog(QWidget* parent = NULL);
	void progressChanged(int value, int max);

private slots:
	void buttonClicked(QAbstractButton* button);

private:
	Ui_PVSDialog ui;
	Task* task;
};

#endif /* PVSDIALOG_H_ */
