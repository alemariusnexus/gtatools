#ifndef COMPOUNDUNDOCOMMAND_H_
#define COMPOUNDUNDOCOMMAND_H_

#include <QtGui/QUndoCommand>
#include <QtCore/QList>


class CompoundUndoCommand : public QUndoCommand
{
public:
	virtual ~CompoundUndoCommand() { for (int i = 0 ; i < cmds.size() ; i++) delete cmds[i]; }
	void addCommand(QUndoCommand* cmd) { cmds << cmd; }
	virtual void redo() { for (int i = 0 ; i < cmds.size() ; i++) cmds[i]->redo(); }
	virtual void undo() { for (int i = cmds.size()-1 ; i >= 0 ; i--) cmds[i]->undo(); }

private:
	QList<QUndoCommand*> cmds;
};

#endif /* COMPOUNDUNDOCOMMAND_H_ */
