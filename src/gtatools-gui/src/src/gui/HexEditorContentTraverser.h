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
