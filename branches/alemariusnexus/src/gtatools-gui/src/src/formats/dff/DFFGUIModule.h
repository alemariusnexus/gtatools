/*
 * DFFGUIModule.h
 *
 *  Created on: 27.07.2010
 *      Author: alemariusnexus
 */

#ifndef DFFGUIMODULE_H_
#define DFFGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <qobject.h>
#include <qaction.h>


class DFFWidget;



class DFFGUIModule : public GUIModule {
	Q_OBJECT

public:
	DFFGUIModule(DFFWidget* dffWidget);

private:
	virtual void doInstall();
	virtual void doUninstall();

private:
	QAction* texturedAction;
	QAction* wireframeAction;
};

#endif /* DFFGUIMODULE_H_ */
