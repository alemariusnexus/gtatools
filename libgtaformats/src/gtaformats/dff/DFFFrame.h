/*
	Copyright 2010 David Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
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
