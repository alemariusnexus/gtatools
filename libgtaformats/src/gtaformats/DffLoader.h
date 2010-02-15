/*
 * DffLoader.h
 *
 *  Created on: 03.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFLOADER_H_
#define DFFLOADER_H_

#include "gf_config.h"
#include "gta.h"
#include "DffMesh.h"
#include "DFFException.h"
#include <istream>
#include <cstdio>

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

struct DffLoadContext {
	DffMesh* mesh;
	//DFFGeometry* currentGeometry;
	int32_t nextGeometryIndex;
	int32_t nextMaterialIndex;
	int32_t nextTextureIndex;
	int32_t nextFrameIndex;
	//int32_t framesRead;
	//int32_t materialIndex;
	//int32_t textureIndex;
	int depth;
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
	int parseSection(istream* stream, RwSectionHeader* parent, DffLoadContext* context);
	int parseStruct(istream* stream, RwSectionHeader& structHeader, RwSectionHeader* parent,
			DffLoadContext* context);
	int parseString(istream* stream, RwSectionHeader& stringHeader, RwSectionHeader* parent,
			DffLoadContext* context);
	int parseFrame(istream* stream, RwSectionHeader& frameHeader, RwSectionHeader* parent,
			DffLoadContext* context);
	int parseMaterialSplit(istream* stream, RwSectionHeader& matsplitHeader, RwSectionHeader* parent,
			DffLoadContext* context);

	void parseGeometry(istream* stream, DffMesh* mesh);
	void parseMaterial(istream* stream, DffMesh* mesh);
	void parseFrameList(istream* stream, DffMesh* mesh, RwSectionHeader& frameListHeader);

	int parseGeometryColors(istream* stream, DffMesh* mesh, DffGeometryStructHeader& header);
	int parseGeometryTexCoords(istream* stream, DffMesh* mesh, DffGeometryStructHeader& header);
	int parseGeometryFaces(istream* stream, DffMesh* mesh, DffGeometryStructHeader& header);
	int parseGeometryPositions(istream* stream, DffMesh* mesh, DffGeometryStructHeader& header);
	int parseGeometryNormals(istream* stream, DffMesh* mesh, DffGeometryStructHeader& header);
	void printHeaderInfo(DffGeometryStructHeader& header);

	inline int readSectionHeaderWithID(istream* stream, RwSectionHeader& header, uint32_t id)
	{
		RwReadSectionHeader(stream, header);

		if (header.id != id) {
			char expected[64];
			char found[64];
			RwGetSectionName(id, expected);
			RwGetSectionName(header.id, found);
			char errmsg[256];
			sprintf(errmsg, "Error: Found section with type %s where %s was expected!\n", found, expected);
			throw DFFException(DFFException::SyntaxError, errmsg);
			//throw IMGException(errmsg, bytesRead);
			// TODO throw exception
		}

		return sizeof(header);
	}
	inline int skipSectionHeaderWithID(istream* stream, uint32_t id)
	{
		RwSectionHeader header;
		readSectionHeaderWithID(stream, header, id);
		return sizeof(header);
	}
	inline int skipSectionWithID(istream* stream, uint32_t id)
	{
		RwSectionHeader header;
		readSectionHeaderWithID(stream, header, id);
		return RwSkipSectionBody(stream, header) + sizeof(header);
	}

private:
	bool verbose;
};

#endif /* DFFLOADER_H_ */
