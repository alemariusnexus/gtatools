#ifndef HEXEDITORDOCUMENT_H_
#define HEXEDITORDOCUMENT_H_

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QLinkedList>
#include <QtCore/QMultiMap>
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
