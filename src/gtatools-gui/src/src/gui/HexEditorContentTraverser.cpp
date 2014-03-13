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

#include "HexEditorContentTraverser.h"





void HexEditorContentTraverser::seek(int offset)
{
	nextBlkIt = doc->blockMarks.upperBound(offset);

	effectiveBlocks.clear();

	for (QLinkedList<HexEditorBlock*>::const_iterator it = doc->blocks.begin() ; it != doc->blocks.end() ; it++) {
		HexEditorBlock* blk = *it;

		if (blk->contains(offset))
			effectiveBlocks << blk;
	}

	this->offset = offset;
}


int HexEditorContentTraverser::advance(int maxDist)
{
	int dist = std::min(getEffectiveLength(), maxDist);

	offset++;

	if (nextBlkIt != doc->blockMarks.end()  &&  offset >= nextBlkIt.key()) {
		QList<HexEditorBlock*> blocksEnded, blocksStarted;

		for (; nextBlkIt != doc->blockMarks.end()  &&  nextBlkIt.key() == offset ; nextBlkIt++) {
			HexEditorDocument::BlockMark mark = nextBlkIt.value();

			if (mark.blockStart) {
				blocksStarted << mark.block;
				effectiveBlocks << mark.block;
			} else {
				blocksEnded << mark.block;
				effectiveBlocks.removeOne(mark.block);
			}
		}

		emit effectiveBlocksChanged(effectiveBlocks, blocksEnded, blocksStarted);
	}

	return dist;
}
