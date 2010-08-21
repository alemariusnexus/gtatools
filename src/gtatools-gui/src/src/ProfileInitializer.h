/*
 * ProfileInitializer.h
 *
 *  Created on: 01.08.2010
 *      Author: alemariusnexus
 */

#ifndef PROFILEINITIALIZER_H_
#define PROFILEINITIALIZER_H_

#include <qthread.h>
#include <gtaformats/util/File.h>

class Profile;


class ProfileInitializer : public QThread {
	Q_OBJECT

public:
	ProfileInitializer(Profile* profile);
	void interrupt() { interrupted = true; }
	bool isInterrupted() { return interrupted; }

protected:
	virtual void run();

private:
	void addResource(const File& file);

private:
	Profile* profile;
	bool interrupted;
};

#endif /* PROFILEINITIALIZER_H_ */
