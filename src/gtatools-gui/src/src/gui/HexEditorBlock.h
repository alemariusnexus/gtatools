#ifndef HEXEDITORBLOCK_H_
#define HEXEDITORBLOCK_H_

#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtCore/QList>



class HexEditorBlock
{
public:
	enum BorderType
	{
		NoBorder = 0,
		ThinBorder = 1,
		MediumBorder = 2,
		ThickBorder = 3
	};

public:
	//HexEditorBlock() : offset(-1), length(-1), borderType(NoBorder), borderColor(QColor(Qt::black)) {}
	HexEditorBlock(int offset, int length);
	HexEditorBlock(const HexEditorBlock& other);
	void setBackgroundBrush(const QBrush& brush) { bgBrush = brush; }
	QBrush getBackgroundBrush() const { return bgBrush; }
	void setBorderType(BorderType type) { borderType = type; }
	BorderType getBorderType() const { return borderType; }
	void setBorderColor(const QColor& color) { borderColor = color; }
	QColor getBorderColor() const { return borderColor; }
	int getOffset() const { return offset; }
	int getLength() const { return length; }
	bool isValid() const { return offset >= 0; }
	bool contains(int offset) const { return offset >= this->offset  &&  offset < this->offset+length; }
	bool overlaps(int offset, int len) const;
	void setSubdivisions(const QList<int>& subdivs) { this->subdivs = subdivs; }
	QList<int> getSubdivisions() const { return subdivs; }
	void getSubdivisionRange(int subdiv, int& offset, int& len);

private:
	int offset;
	int length;
	QList<int> subdivs;

	QBrush bgBrush;
	BorderType borderType;
	QColor borderColor;
};

#endif /* HEXEDITORBLOCK_H_ */
