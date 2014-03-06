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

