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

#ifndef HEXEDITORCONTENTTRAVERSER_H_
#define HEXEDITORCONTENTTRAVERSER_H_

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMultiMap>
#include "HexEditorBlock.h"
#include "HexEditorDocument.h"



class HexEditorContentTraverser : public QObject
{
	Q_OBJECT

public:
	HexEditorContentTraverser(const HexEditorContentTraverser& other)
			: doc(other.doc), offset(other.offset), effectiveBlocks(other.effectiveBlocks), nextBlkIt(other.nextBlkIt) {}
	QList<HexEditorBlock*> getEffectiveBlocks() const { return effectiveBlocks; }
	void seek(int offset);
	int advance(int maxDist = 1);
	int getEffectiveLength() const { return nextBlkIt == doc->blockMarks.end() ? doc->size()-offset : nextBlkIt.key()-offset; }

private:
	HexEditorContentTraverser(const HexEditorDocument* doc, int offset)
			: doc(doc)
	{ seek(offset); }

signals:
	void effectiveBlocksChanged(QList<HexEditorBlock*> effectiveBlocks, QList<HexEditorBlock*> blocksEnded,
			QList<HexEditorBlock*> blocksStarted);

private:
	const HexEditorDocument* doc;
	int offset;
	QList<HexEditorBlock*> effectiveBlocks;
	QMultiMap<int, HexEditorDocument::BlockMark>::const_iterator nextBlkIt;

	friend class HexEditorDocument;
};

#endif /* HEXEDITORCONTENTTRAVERSER_H_ */
