#ifndef BATCHINGUNDODECORATOR_H_
#define BATCHINGUNDODECORATOR_H_

#include "UndoDecorator.h"
#include "CompoundUndoCommand.h"



class BatchingUndoDecorator : public UndoDecorator
{
public:
	BatchingUndoDecorator();
	virtual void push(QUndoCommand* cmd);
	void startBatch();
	void endBatch();
	void blockUndo();
	void unblockUndo();

private:
	CompoundUndoCommand* currentCompoundCmd;
	bool blocked;
};

#endif /* BATCHINGUNDODECORATOR_H_ */
