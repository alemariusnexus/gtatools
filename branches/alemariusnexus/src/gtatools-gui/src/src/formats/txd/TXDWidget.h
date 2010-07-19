/*
 * TXDWidget.h
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#ifndef TXDWIDGET_H_
#define TXDWIDGET_H_

#include <ui_TXDWidget.h>
#include <gtaformats/util/File.h>
#include <qwidget.h>
#include <qlistwidget.h>
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include "OpenTXDGUIModule.h"
#include <qlinkedlist.h>



class TXDWidget : public QWidget {
	Q_OBJECT

public:
	TXDWidget(const File& file, QWidget* parent);
	~TXDWidget();
	TXDArchive* getArchive() { return txd; }
	QLinkedList<TXDTexture*> getSelectedTextures();

private:
	void loadConfigUiSettings();

private slots:
	void textureActivated(QListWidgetItem* item);
	void configurationChanged();

private:
	OpenTXDGUIModule* openGUIModule;
	QTabWidget* compactTab;
	Ui_TXDWidget ui;
	TXDArchive* txd;
	TXDTexture** textures;
};

#endif /* TXDWIDGET_H_ */
