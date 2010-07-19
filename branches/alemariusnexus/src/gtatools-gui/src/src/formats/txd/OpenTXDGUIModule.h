/*
 * OpenTXDGUIModule.h
 *
 *  Created on: 18.07.2010
 *      Author: alemariusnexus
 */

#ifndef OPENTXDGUIMODULE_H_
#define OPENTXDGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <qaction.h>
#include <qlinkedlist.h>


class TXDWidget;


struct ImageFormat
{
	QString* name;
	QLinkedList<QString*>* extensions;
};


class OpenTXDGUIModule : public GUIModule {
	Q_OBJECT

public:
	OpenTXDGUIModule(TXDWidget* txdWidget);
	~OpenTXDGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void onExtract(bool checked);

private:
	QLinkedList<ImageFormat*> supportedImageFormats;
	TXDWidget* txdWidget;
	QAction* extractAction;
};

#endif /* OPENTXDGUIMODULE_H_ */
