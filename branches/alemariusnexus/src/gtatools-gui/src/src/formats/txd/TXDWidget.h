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



class TXDWidget : public QWidget {
	Q_OBJECT

public:
	TXDWidget(const File& file, QWidget* parent);
	~TXDWidget();

private slots:
	void textureActivated(QListWidgetItem* item);

private:
	Ui_TXDWidget ui;
	TXDArchive* txd;
	TXDTexture** textures;
};

#endif /* TXDWIDGET_H_ */
