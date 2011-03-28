/*
 * MainApplication.h
 *
 *  Created on: 28.03.2011
 *      Author: alemariusnexus
 */

#ifndef MAINAPPLICATION_H_
#define MAINAPPLICATION_H_

#include <QtGui/QApplication>


class MainApplication : public QApplication
{
	Q_OBJECT

public:
	MainApplication(int& argc, char** argv) : QApplication(argc, argv) {}
	virtual bool notify(QObject* receiver, QEvent* event);
};

#endif /* MAINAPPLICATION_H_ */
