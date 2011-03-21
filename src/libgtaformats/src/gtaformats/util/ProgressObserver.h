/*
 * ProgressObserver.h
 *
 *  Created on: 21.03.2011
 *      Author: alemariusnexus
 */

#ifndef PROGRESSOBSERVER_H_
#define PROGRESSOBSERVER_H_

class ProgressObserver
{
public:
	virtual void progressChanged(int value, int max) = 0;
};

#endif /* PROGRESSOBSERVER_H_ */
