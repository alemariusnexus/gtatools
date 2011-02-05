/*
 * COLGUIModule.h
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#ifndef COLGUIMODULE_H_
#define COLGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <QtGui/QAction>


class COLGUIModule : public GUIModule {
	Q_OBJECT

public:
	static COLGUIModule* getInstance() { static COLGUIModule inst; return &inst; }

signals:
	void wireframePropertyChanged(bool wireframe);

private slots:
	void wireframePropertyChangedSlot(bool wireframe);

private:
	COLGUIModule();
	virtual void doInstall();
	virtual void doUninstall();

private:
	QAction* wireframeAction;
};

#endif /* COLGUIMODULE_H_ */
