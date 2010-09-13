/*
 * SharedData.h
 *
 *  Created on: 21.08.2010
 *      Author: alemariusnexus
 */

#ifndef SHAREDDATA_H_
#define SHAREDDATA_H_

template<class T>
class SharedDataPointer;


template<class T>
class SharedData {
public:
	SharedData(T* data);
	~SharedData();
	T* getData() { return data; }
	SharedDataPointer<T> operator&();


private:
	void incrementReferenceCount() { referenceCount++; }
	void decrementReferenceCount() { referenceCount--; }
	int getReferenceCount() { return referenceCount; }

private:
	T* data;
	int referenceCount;

public:
	friend class SharedDataPointer<T>;
};

#endif /* SHAREDDATA_H_ */
