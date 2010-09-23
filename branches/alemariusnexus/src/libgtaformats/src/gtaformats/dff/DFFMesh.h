/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#ifndef DFFMODEL_H_
#define DFFMODEL_H_

#include "../gf_config.h"
#include "DFFGeometry.h"
#include "DFFFrame.h"
#include <vector>

using std::vector;




class DFFMesh {
private:
	friend class DFFLoader;

public:
	typedef vector<DFFFrame*>::iterator FrameIterator;
	typedef vector<DFFFrame*>::const_iterator ConstFrameIterator;
	typedef vector<DFFGeometry*>::iterator GeometryIterator;
	typedef vector<DFFGeometry*>::const_iterator ConstGeometryIterator;

public:
	DFFMesh() {};
	DFFMesh(const DFFMesh& other);
	~DFFMesh();
	int32_t getFrameCount() const { return frames.size(); }
	FrameIterator getFrameBegin() { return frames.begin(); }
	ConstFrameIterator getFrameBegin() const { return frames.begin(); }
	FrameIterator getFrameEnd() { return frames.end(); }
	ConstFrameIterator getFrameEnd() const { return frames.end(); }
	DFFFrame* getFrame(int32_t idx) { return frames[idx]; }
	const DFFFrame* getFrame(int32_t idx) const { return frames[idx]; }
	DFFFrame* getFrame(const char* name);
	const DFFFrame* getFrame(const char* name) const;
	int32_t getGeometryCount() const { return geometries.size(); }
	GeometryIterator getGeometryBegin() { return geometries.begin(); }
	ConstGeometryIterator getGeometryBegin() const { return geometries.begin(); }
	GeometryIterator getGeometryEnd() { return geometries.end(); }
	ConstGeometryIterator getGeometryEnd() const { return geometries.end(); }
	DFFGeometry* getGeometry(int32_t idx) { return geometries[idx]; }
	const DFFGeometry* getGeometry(int32_t idx) const { return geometries[idx]; }
	DFFGeometry* getGeometry(const char* name);
	const DFFGeometry* getGeometry(const char* name) const;
	int32_t indexOf(const DFFFrame* frame) const;
	void addFrame(DFFFrame* frame) { frames.push_back(frame); }
	void removeFrame(DFFFrame* frame);
	void removeFrame(int32_t index) { removeFrame(frames[index]); }
	void removeFrame(const char* name) { removeFrame(getFrame(name)); }
	void removeFrames();
	void addGeometry(DFFGeometry* geom) { geometries.push_back(geom); }
	void removeGeometry(DFFGeometry* geom);
	void removeGeometry(int32_t index) { removeGeometry(geometries[index]); }
	void removeGeometry(const char* name) { removeGeometry(getGeometry(name)); }
	void removeGeometries();

	void mirrorYZ();
	void scale(float x, float y, float z);

private:
	vector<DFFFrame*> frames;
	vector<DFFGeometry*> geometries;
};

#endif /* DFFMODEL_H_ */
