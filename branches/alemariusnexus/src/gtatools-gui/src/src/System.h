/*
 * System.h
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <qobject.h>
#include <gtaformats/util/File.h>



class System : public QObject {
	Q_OBJECT

public:
	static System* getInstance();

public:
	void openFile(const File& file);
	void closeCurrentFile();
	bool hasOpenFile() { return fileOpen; }

signals:
	void fileOpened(const File& file);
	void currentFileClosed();

private:
	System() {}

private:
	bool fileOpen;
};

#endif /* SYSTEM_H_ */
