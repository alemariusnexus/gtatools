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
