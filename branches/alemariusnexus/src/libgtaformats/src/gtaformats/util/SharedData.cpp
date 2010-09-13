/*
 * SharedData.cpp
 *
 *  Created on: 21.08.2010
 *      Author: alemariusnexus
 */

#include "SharedData.h"
#include "SharedDataPointer.h"


template<class T>
SharedData<T>::SharedData(T* data)
		: data(data)
{
}


template<class T>
SharedData<T>::~SharedData()
{
	delete data;
}


template<class T>
SharedDataPointer<T> SharedData<T>::operator&()
{
	return SharedDataPointer<T>(this);
}

