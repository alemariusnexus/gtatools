/*
 * RWBSGUIModule.h
 *
 *  Created on: 21.06.2011
 *      Author: alemariusnexus
 */

#ifndef RWBSGUIMODULE_H_
#define RWBSGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include "../../gui/HexEditor.h"
#include <QtGui/QAction>


class RWBSGUIModule : public GUIModule {
	Q_OBJECT

public:
	static RWBSGUIModule* getInstance() { static RWBSGUIModule* inst = new RWBSGUIModule; return inst; }

public:
	void setEditor(HexEditor* editor) { this->editor = editor; }

private:
	RWBSGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void gotoRequested();

private:
	QAction* gotoAction;
	HexEditor* editor;
};

#endif /* RWBSGUIMODULE_H_ */
