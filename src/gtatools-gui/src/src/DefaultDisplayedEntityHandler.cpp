#include "DefaultDisplayedEntityHandler.h"



bool DefaultDisplayedEntityHandler::doSaveChanges()
{
	bool success = false;
	emit handleSaveChangesRequest(success);
	return success;
}
