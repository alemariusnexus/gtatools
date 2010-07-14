/*
 * TXDGUIModule.h
 *
 *  Created on: 14.07.2010
 *      Author: alemariusnexus
 */

#ifndef TXDGUIMODULE_H_
#define TXDGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <qaction.h>



class TXDGUIModule : public GUIModule {
	Q_OBJECT

public:
	virtual void apply(MainWindow* mw);
	virtual void remove(MainWindow* mw);

private slots:
	void onFindTexture(bool checked);

private:
	QAction* findTextureAction;
};

#endif /* TXDGUIMODULE_H_ */
