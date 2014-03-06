#include "UndoDecorator.h"



void UndoDecorator::commit(QUndoCommand* cmd)
{
	emit committed(cmd);
}
