/*
 * GUIModule.h
 *
 *  Created on: 14.07.2010
 *      Author: alemariusnexus
 */

#ifndef GUIMODULE_H_
#define GUIMODULE_H_

class MainWindow;

#include <qobject.h>



class GUIModule : public QObject {
	Q_OBJECT

public:
	virtual void apply(MainWindow* mw) = 0;
	virtual void remove(MainWindow* mw) = 0;
};

#endif /* GUIMODULE_H_ */
