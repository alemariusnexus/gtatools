#ifndef UNDODECORATOR_H_
#define UNDODECORATOR_H_

#include <QtGui/QUndoCommand>


class UndoDecorator : public QObject
{
	Q_OBJECT

public:
	virtual void push(QUndoCommand* cmd) = 0;

protected:
	void commit(QUndoCommand* cmd);

signals:
	void committed(QUndoCommand* cmd);
};

#endif /* UNDODECORATOR_H_ */
