/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef TXDFORMATHANDLER_H_
#define TXDFORMATHANDLER_H_

#include "../FormatHandler.h"
#include <QtGui/QImage>
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTextureHeader.h>
#include <gtaformats/config.h>
#include <QtCore/QLinkedList>
#include <QtGui/QWidget>
#include <QtGui/QImage>
#include "../../System.h"



class TXDFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	static TXDFormatHandler* getInstance();

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("Texture Dictionary (TXD)"); };
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "txd"; }
	virtual bool canHandle(const EntityOpenRequest& req) const;
	virtual DisplayedEntity* openEntity(const EntityOpenRequest& request);

	bool extractTexturesDialog(TXDArchive* txd, const QLinkedList<TXDTextureHeader*>& texes, QWidget* parent);
	bool findTextureDialog(const QLinkedList<File*>& files, QWidget* parent);
	QImage createImageFromTexture(TXDTextureHeader* tex, uint8_t* data, uint8_t*& resultData);

private slots:
	void systemQuerySent(const SystemQuery& query, SystemQueryResult& result);

private:
	TXDFormatHandler();
};

#endif /* TXDFORMATHANDLER_H_ */
