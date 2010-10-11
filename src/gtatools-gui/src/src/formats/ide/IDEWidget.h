/*
 * IDEWidget.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef IDEWIDGET_H_
#define IDEWIDGET_H_

#include <QtGui/QWidget>
#include <ui_IDEWidget.h>
#include <gtaformats/util/File.h>



class IDEWidget : public QWidget {
	Q_OBJECT

public:
	IDEWidget(QWidget* parent, const File& file);

private:
	Ui_IDEWidget ui;
};

#endif /* IDEWIDGET_H_ */
