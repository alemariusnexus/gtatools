/*
 * IFPFormatHandler.h
 *
 *  Created on: 02.01.2012
 *      Author: alemariusnexus
 */

#ifndef IFPFORMATHANDLER_H_
#define IFPFORMATHANDLER_H_

#include "../FormatHandler.h"
#include "IFPGUIModule.h"


class IFPFormatHandler : public FormatHandler
{
	Q_OBJECT

public:
	IFPFormatHandler();
	virtual QString getFormatName(const File* file = NULL) const { return tr("IFP Animation Package"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "ifp"; }
	virtual bool canHandle(const EntityOpenRequest& req) const;
	virtual DisplayedEntity* openEntity(const EntityOpenRequest& request);

private:
	IFPGUIModule* guiModule;
};

#endif /* IFPFORMATHANDLER_H_ */
