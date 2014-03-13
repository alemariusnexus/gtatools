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

#include "HexEditorDocument.h"
#include "HexEditorContentTraverser.h"
#include <algorithm>



HexEditorDocument::HexEditorDocument(const QByteArray& data)
		: data(data)
{
}


HexEditorDocument::HexEditorDocument()
		: data(QByteArray())
{
}


void HexEditorDocument::setData(const QByteArray& data)
{
	QByteArray oldData = this->data;
	this->data = data;

	emit dataReplaced(0, oldData.size(), oldData, data);
	emit dataChanged(data);
}


void HexEditorDocument::insert(int offset, const QByteArray& data)
{
	replace(offset, 0, data);
}


void HexEditorDocument::remove(int offset, int len)
{
	replace(offset, len, QByteArray());
}


void HexEditorDocument::replace(int offset, int len, const QByteArray& data)
{
	QByteArray oldData = this->data.mid(offset, len);
	this->data.replace(offset, len, data);

	emit dataReplaced(offset, len, oldData, data);
	emit dataChanged(data);
}


void HexEditorDocument::addBlock(HexEditorBlock* block)
{
	blocks << block;

	BlockMark beg;
	beg.block = block;
	beg.blockStart = true;

	BlockMark end;
	end.block = block;
	end.blockStart = false;

	blockMarks.insert(block->getOffset(), beg);
	blockMarks.insert(block->getOffset()+block->getLength(), end);
}


void HexEditorDocument::clearBlocks()
{
	blocks.clear();
	blockMarks.clear();
}


HexEditorContentTraverser* HexEditorDocument::getContentTraverser(int offset) const
{
	HexEditorContentTraverser* trv = new HexEditorContentTraverser(this, offset);
	return trv;
}


QList<HexEditorBlock*> HexEditorDocument::getEffectiveBlocks(int offset, int len)
{
	QList<HexEditorBlock*> blocks;

	for (QLinkedList<HexEditorBlock*>::iterator it = this->blocks.begin() ; it != this->blocks.end() ; it++) {
		HexEditorBlock* block = *it;

		if (block->overlaps(offset, len))
			blocks << block;
	}

	return blocks;
}

