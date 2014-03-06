#ifndef HEXEDITORUNDOCOMMAND_H_
#define HEXEDITORUNDOCOMMAND_H_

#include <QtCore/QObject>
#include <QtGui/QUndoCommand>
#include "HexEditor.h"


class HexEditorUndoCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	HexEditorUndoCommand(HexEditor* editor, int offset, int len, const QByteArray& oldData, const QByteArray& newData,
			bool alreadyDone);

	virtual void undo();
	virtual void redo();

private:
	HexEditor* editor;
	int offset;
	int len;
	QByteArray oldData;
	QByteArray newData;
	bool alreadyDone;
};

#endif /* HEXEDITORUNDOCOMMAND_H_ */
