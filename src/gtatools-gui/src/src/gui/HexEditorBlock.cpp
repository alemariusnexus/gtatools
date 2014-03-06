#include "HexEditorBlock.h"



HexEditorBlock::HexEditorBlock(int offset, int length)
		: offset(offset), length(length), bgBrush(QBrush(QColor(QColor()))), borderType(NoBorder),
		  borderColor(QColor(Qt::black))
{
}


HexEditorBlock::HexEditorBlock(const HexEditorBlock& other)
		: offset(other.offset), length(other.length), bgBrush(other.bgBrush), borderType(other.borderType),
		  borderColor(other.borderColor)
{
}


bool HexEditorBlock::overlaps(int offset, int len) const
{
	if (offset < this->offset)
		return offset+len > this->offset;
	else
		return offset < this->offset+this->length;
}


void HexEditorBlock::getSubdivisionRange(int subdiv, int& offset, int& len)
{
	offset = subdiv == 0 ? 0 : subdivs[subdiv-1];
	len = subdiv >= subdivs.size() ? len-offset : subdivs[subdiv]-offset;
}
