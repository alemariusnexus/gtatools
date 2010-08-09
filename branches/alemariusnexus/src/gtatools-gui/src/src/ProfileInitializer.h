/*
 * ProfileInitializer.h
 *
 *  Created on: 01.08.2010
 *      Author: alemariusnexus
 */

#ifndef PROFILEINITIALIZER_H_
#define PROFILEINITIALIZER_H_

#include <qthread.h>

class Profile;


class ProfileInitializer : public QThread {
	Q_OBJECT

public:
	ProfileInitializer(Profile* profile);

protected:
	virtual void run();

private:
	Profile* profile;
};

#endif /* PROFILEINITIALIZER_H_ */
