/*
 * ProfileConfigWidget.h
 *
 *  Created on: 12.07.2010
 *      Author: alemariusnexus
 */

#ifndef PROFILECONFIGWIDGET_H_
#define PROFILECONFIGWIDGET_H_

#include "ui_ProfileConfigWidget.h"
#include <qwidget.h>
#include <qlinkedlist.h>
#include "../Profile.h"



class ProfileConfigWidget : public QWidget {
	Q_OBJECT

public:
	ProfileConfigWidget(QWidget* parent = NULL);
	~ProfileConfigWidget();
	QString getProfileName() { return ui.nameField->text(); }
	void getFiles(QLinkedList<QString>& dest);
	void displayProfile(Profile* profile);

private slots:
	void fileAddButtonClicked(bool checked = false);
	void dirAddButtonClicked(bool checked = false);
	void fileEditButtonClicked(bool checked = false);
	void fileRemoveButtonClicked(bool checked = false);

private:
	void clearFiles();

private:
	Ui_ProfileConfigWidget ui;
};

#endif /* PROFILECONFIGWIDGET_H_ */
