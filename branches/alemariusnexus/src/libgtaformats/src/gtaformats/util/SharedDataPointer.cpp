/*
 * SharedDataPointer.cpp
 *
 *  Created on: 21.08.2010
 *      Author: alemariusnexus
 */

#include "SharedDataPointer.h"


template<class T>
SharedDataPointer<T>::SharedDataPointer(SharedData<T>* data)
		: data(data)
{
	data->incrementReferenceCount();
}


template<class T>
SharedDataPointer<T>::~SharedDataPointer()
{
	data->decrementReferenceCount();

	if (data->getReferenceCount() == 0) {
		delete data;
	}
}


template<class T>
T* SharedDataPointer<T>::operator*()
{
	return data->data;
}

