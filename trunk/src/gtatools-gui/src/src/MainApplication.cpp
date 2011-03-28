/*
 * MainApplication.cpp
 *
 *  Created on: 28.03.2011
 *      Author: alemariusnexus
 */

#include "MainApplication.h"
#include <gtaformats/util/Exception.h>
#include "System.h"



bool MainApplication::notify(QObject* receiver, QEvent* event)
{
	try {
		return QApplication::notify(receiver, event);
	} catch (Exception& ex) {
		System* sys = System::getInstance();
		sys->unhandeledException(ex);
		return false;
	}
}
