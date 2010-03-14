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

#ifndef DFFLOADER_H_
#define DFFLOADER_H_

#include "../gf_config.h"
#include "../gta.h"
#include "DFFMesh.h"
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

#define DFF_VERSION_GTA3_1 0
#define DFF_VERSION_GTA3_2 2048
#define DFF_VERSION_GTA3_3 65400
#define DFF_VERSION_GTAVC_1 3074
#define DFF_VERSION_GTAVC_2 4099
#define DFF_VERSION_GTASA 6147



struct DFFGeometryStructHeader {
	int16_t flags;
	uint8_t uvSetCount;
	uint8_t _unknown;
	int32_t faceCount;
	int32_t vertexCount;
	int32_t frameCount;
};

struct DFFLoadContext {
	DFFMesh* mesh;
	int32_t version;
	int32_t nextGeometryIndex;
	int32_t nextMaterialIndex;
	int32_t nextTextureIndex;
	int32_t nextFrameIndex;
	int depth;
};


class DFFLoader {
public:
	DFFLoader();
	virtual ~DFFLoader();
	DFFMesh* loadMesh(istream* stream);
	void setVerbose(bool verbose) { this->verbose = verbose; }
	bool isVerbose() { return verbose; }

public:


protected:


private:
	int parseSection(istream* stream, RwSectionHeader* parent, DFFLoadContext* context);
	int parseStruct(istream* stream, RwSectionHeader& structHeader, RwSectionHeader* parent,
			DFFLoadContext* context);
	int parseString(istream* stream, RwSectionHeader& stringHeader, RwSectionHeader* parent,
			DFFLoadContext* context);
	int parseFrame(istream* stream, RwSectionHeader& frameHeader, RwSectionHeader* parent,
			DFFLoadContext* context);
	int parseMaterialSplit(istream* stream, RwSectionHeader& matsplitHeader, RwSectionHeader* parent,
			DFFLoadContext* context);

	void parseGeometry(istream* stream, DFFMesh* mesh);
	void parseMaterial(istream* stream, DFFMesh* mesh);
	void parseFrameList(istream* stream, DFFMesh* mesh, RwSectionHeader& frameListHeader);

	void printHeaderInfo(DFFGeometryStructHeader& header, DFFLoadContext* context);

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
