/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef HEXEDITORDOCUMENT_H_
#define HEXEDITORDOCUMENT_H_

#include <QObject>
#include <QByteArray>
#include <QLinkedList>
#include <QMultiMap>
#include "HexEditorBlock.h"



class HexEditorContentTraverser;


class HexEditorDocument : public QObject
{
	Q_OBJECT

private:
	struct BlockMark
	{
		HexEditorBlock* block;
		bool blockStart;
	};

public:
	HexEditorDocument(const QByteArray& data);
	HexEditorDocument();
	QByteArray getData() const { return data; }
	void setData(const QByteArray& data);
	size_t size() const { return data.size(); }

	void insert(int offset, const QByteArray& data);
	void insert(int offset, char c) { insert(offset, QByteArray(1, c)); }
	void insert(const QByteArray& data);
	void remove(int offset, int len);
	void replace(int offset, int len, const QByteArray& data);
	void replace(int offset, const QByteArray& data) { replace(offset, data.size(), data); }
	void replace(int offset, char c) { replace(offset, QByteArray(1, c)); }
	void prepend(const QByteArray& data) { insert(0, data); }
	void append(const QByteArray& data) { insert(this->data.size(), data); }

	void addBlock(HexEditorBlock* block);
	void clearBlocks();

	HexEditorContentTraverser* getContentTraverser(int offset = 0) const;
	QList<HexEditorBlock*> getEffectiveBlocks(int offset, int len);

signals:
	void dataChanged(const QByteArray& data);
	void dataReplaced(int offset, int len, const QByteArray& oldData, const QByteArray& newData);

private:
	QByteArray data;
	QLinkedList<HexEditorBlock*> blocks;
	QMultiMap<int, BlockMark> blockMarks;

	friend class HexEditorContentTraverser;
};

#endif /* HEXEDITORDOCUMENT_H_ */
