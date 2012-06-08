/*
 * IFPGUIModule.h
 *
 *  Created on: 27.05.2012
 *      Author: alemariusnexus
 */

#ifndef IFPGUIMODULE_H_
#define IFPGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <QtGui/QAction>


class IFPGUIModule : public GUIModule
{
	Q_OBJECT

public:
	IFPGUIModule();
	virtual ~IFPGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void animViewerRequested(bool);

private:
	QAction* animViewerAction;
};

#endif /* IFPGUIMODULE_H_ */
