#ifndef NULLUNDODECORATOR_H_
#define NULLUNDODECORATOR_H_

#include "UndoDecorator.h"



class NullUndoDecorator : public UndoDecorator
{
public:
	virtual void push(QUndoCommand* cmd) { commit(cmd); }
};

#endif /* NULLUNDODECORATOR_H_ */
