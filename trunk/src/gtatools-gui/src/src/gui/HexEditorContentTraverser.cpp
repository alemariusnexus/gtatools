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
