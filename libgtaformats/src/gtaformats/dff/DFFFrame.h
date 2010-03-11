/*
 * DFFFrame.h
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#ifndef DFFFRAME_H_
#define DFFFRAME_H_

#include "../gf_config.h"


class DFFFrame {
private:
	friend class DFFLoader;

public:
	DFFFrame() : name(NULL) {}
	~DFFFrame();
	const float* getRotation() const { return &rotation[0]; }
	const float* getTranslation() const { return &translation[0]; }
	DFFFrame* getParent() const { return parent; }
	int32_t getFlags() const { return flags; }
	char* getName() const { return name; }

	void mirrorYZ();

private:
	float rotation[9];
	float translation[3];
	DFFFrame* parent;
	int32_t flags;
	char* name;
};

#endif /* DFFFRAME_H_ */
