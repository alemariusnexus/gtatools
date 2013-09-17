#ifndef SYSTEMQUERYHANDLER_H_
#define SYSTEMQUERYHANDLER_H_

#include <QtCore/QObject>
#include "SystemQueryResult.h"



class SystemQueryHandler : public QObject
{
	Q_OBJECT

public:
	virtual void handleQuery();
};

#endif /* SYSTEMQUERYHANDLER_H_ */
