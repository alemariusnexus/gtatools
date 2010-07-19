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
	TXDGUIModule();
	virtual ~TXDGUIModule();
	virtual void buildFileTreeMenu(const File& file, QMenu& menu);

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void onFindTexture(bool checked);
	void onFindTextureInFile(bool checked);

private:
	File* contextFile;
	QAction* findTextureAction;
	QAction* findTextureInFileAction;
};

#endif /* TXDGUIMODULE_H_ */
