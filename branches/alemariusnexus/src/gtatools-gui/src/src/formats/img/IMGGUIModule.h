/*
 * IMGGUIModule.h
 *
 *  Created on: 13.08.2010
 *      Author: alemariusnexus
 */

#ifndef IMGGUIMODULE_H_
#define IMGGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <qaction.h>
#include <qhash.h>
#include <qstring.h>
#include <qvariant.h>


class IMGGUIModule : public GUIModule {
	Q_OBJECT

public:
	IMGGUIModule();
	virtual ~IMGGUIModule();
	virtual void buildFileTreeMenu(const File& file, QMenu& menu);
	void openFileChanged(const File& file);

private:
	virtual void doInstall();
	virtual void doUninstall();
	void extract(const File& file);

private slots:
	void onExtract(bool);
	void onContextExtract(bool);
	void currentFileChanged(File* file, File* prev);

private:
	QAction* extractAction;
	QAction* contextExtractAction;
	File* currentContextFile;
	bool menuExists;
};

#endif /* IMGGUIMODULE_H_ */
