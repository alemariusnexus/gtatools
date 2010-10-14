/*
 * IDESearchWidget.h
 *
 *  Created on: 15.10.2010
 *      Author: alemariusnexus
 */

#ifndef IDESEARCHWIDGET_H_
#define IDESEARCHWIDGET_H_

#include <ui_IDESearchWidget.h>
#include "IDEFileFinder.h"



class IDESearchWidget : public QDialog {
	Q_OBJECT

public:
	IDESearchWidget(QWidget* parent = NULL);

private slots:
	void onCancel(bool checked);
	void onSearch(bool checked);

private:
	Ui_IDESearchWidget ui;
	IDEFileFinder* finder;
};

#endif /* IDESEARCHWIDGET_H_ */
