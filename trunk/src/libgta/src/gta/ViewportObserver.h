/*
 * ViewportObserver.h
 *
 *  Created on: 08.06.2012
 *      Author: alemariusnexus
 */

#ifndef VIEWPORTOBSERVER_H_
#define VIEWPORTOBSERVER_H_

class ViewportObserver
{
public:
	virtual void viewportChanged(int width, int height) = 0;
};

#endif /* VIEWPORTOBSERVER_H_ */
