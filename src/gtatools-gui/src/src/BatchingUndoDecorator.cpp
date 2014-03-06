#include "BatchingUndoDecorator.h"




BatchingUndoDecorator::BatchingUndoDecorator()
		: currentCompoundCmd(NULL), blocked(false)
{
}


void BatchingUndoDecorator::push(QUndoCommand* cmd)
{
	if (blocked) {
		cmd->redo();
		delete cmd;
		return;
	}

	if (currentCompoundCmd) {
		currentCompoundCmd->addCommand(cmd);
	} else {
		commit(cmd);
	}
}


void BatchingUndoDecorator::startBatch()
{
	CompoundUndoCommand* cmd = new CompoundUndoCommand;

	if (currentCompoundCmd) {
		cmd->addCommand(currentCompoundCmd);
	}

	currentCompoundCmd = cmd;
}


void BatchingUndoDecorator::endBatch()
{
	if (currentCompoundCmd) {
		commit(currentCompoundCmd);
		currentCompoundCmd = NULL;
	}
}


void BatchingUndoDecorator::blockUndo()
{
	blocked = true;
}


void BatchingUndoDecorator::unblockUndo()
{
	blocked = false;
}
