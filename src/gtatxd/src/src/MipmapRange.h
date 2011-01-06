/*
 * MipmapRange.h
 *
 *  Created on: 03.01.2011
 *      Author: alemariusnexus
 */

#ifndef MIPMAPRANGE_H_
#define MIPMAPRANGE_H_

class MipmapRange {
public:
	MipmapRange(int start, int end) : start(start), end(end) {}
	MipmapRange(int single) : start(single), end(single) {}
	int getStart() const { return start; }
	int getEnd() const { return end; }
	bool contains(int mipmap) const { return mipmap >= start  &&  mipmap <= end; }

private:
	int start, end;
};

#endif /* MIPMAPRANGE_H_ */
