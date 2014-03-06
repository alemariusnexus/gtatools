#ifndef DEFAULTDISPLAYEDENTITYHANDLER_H_
#define DEFAULTDISPLAYEDENTITYHANDLER_H_

#include "DisplayedEntityHandler.h"


class DefaultDisplayedEntityHandler : public DisplayedEntityHandler
{
	Q_OBJECT

public:
	DefaultDisplayedEntityHandler(DisplayedEntity* entity) : DisplayedEntityHandler(entity) {}

protected:
	virtual bool doSaveChanges();

signals:
	void handleSaveChangesRequest(bool& success);
};

#endif /* DEFAULTDISPLAYEDENTITYHANDLER_H_ */
