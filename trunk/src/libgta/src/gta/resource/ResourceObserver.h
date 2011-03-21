/*
 * ResourceObserver.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef RESOURCEOBSERVER_H_
#define RESOURCEOBSERVER_H_

#include <gtaformats/util/File.h>


class ResourceObserver {
public:
	virtual void resourceAdded(const File& file) = 0;
	virtual void resourcesCleared() {};
};

#endif /* RESOURCEOBSERVER_H_ */
