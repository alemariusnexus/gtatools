/*
 * SharedDataPointer.h
 *
 *  Created on: 21.08.2010
 *      Author: alemariusnexus
 */

#ifndef SHAREDDATAPOINTER_H_
#define SHAREDDATAPOINTER_H_

#include "SharedData.h"


template<class T>
class SharedDataPointer {
public:
	T* operator*();

private:
	SharedDataPointer(SharedData<T>* data);
	~SharedDataPointer();

private:
	SharedData<T>* data;

public:
	friend class SharedData<T>;
};

#endif /* SHAREDDATAPOINTER_H_ */
