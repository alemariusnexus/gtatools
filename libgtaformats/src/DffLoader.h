/*
 * DffLoader.h
 *
 *  Created on: 03.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFLOADER_H_
#define DFFLOADER_H_

#include "config.h"
#include "gta.h"
#include "DffMesh.h"
#include <istream>

using std::istream;


#define GEOMETRY_FLAG_TRISTRIP (1<<0)
#define GEOMETRY_FLAG_POSITIONS (1<<1)
#define GEOMETRY_FLAG_TEXCOORDS (1<<2)
#define GEOMETRY_FLAG_COLORS (1<<3)
#define GEOMETRY_FLAG_NORMALS (1<<4)
#define GEOMETRY_FLAG_UNKNOWN1 (1<<5)
#define GEOMETRY_FLAG_UNKNOWN2 (1<<6)
#define GEOMETRY_FLAG_MULTIPLEUVSETS (1<<7)

struct DffGeometryStructHeader {
	int16_t flags;
	uint8_t uvSetCount;
	uint8_t _unknown;
	int32_t faceCount;
	int32_t vertexCount;
	int32_t frameCount;
};


class DffLoader {
public:
	DffLoader();
	virtual ~DffLoader();
	DffMesh* loadMesh(istream* stream);
	void setVerbose(bool verbose) { this->verbose = verbose; }
	bool isVerbose() { return verbose; }

protected:


private:
	void parseGeometry(istream* stream, DffMesh* mesh);
	void parseMaterial(istream* stream, DffMesh* mesh);

private:
	bool verbose;
};

#endif /* DFFLOADER_H_ */
