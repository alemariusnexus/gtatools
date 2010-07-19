/*
 * ConfigWidget.h
 *
 *  Created on: 13.07.2010
 *      Author: alemariusnexus
 */

#ifndef CONFIGWIDGET_H_
#define CONFIGWIDGET_H_

#include <ui_ConfigWidget.h>
#include <qwidget.h>



class ConfigWidget : public QWidget {
	Q_OBJECT

public:
	ConfigWidget(QWidget* parent = NULL);

public slots:
	void selectedProfileChanged(int index);
	void onApply(bool checked);
	void onCancel(bool checked);

public:
	Ui_ConfigWidget ui;
};

#endif /* CONFIGWIDGET_H_ */
