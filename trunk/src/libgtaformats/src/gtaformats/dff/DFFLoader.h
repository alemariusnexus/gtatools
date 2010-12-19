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

#ifndef DFFLOADER_H_
#define DFFLOADER_H_

#include "../gf_config.h"
#include "../gta.h"
#include "DFFMesh.h"
#include "DFFException.h"
#include <cstdio>
#include "../util/stream/InputStream.h"
#include "../util/File.h"
#include "../util/thread/Mutex.h"
#include <map>

using std::map;


#define DFF_VERSION_GTA3_1 0
#define DFF_VERSION_GTA3_2 2048
#define DFF_VERSION_GTA3_3 65400
#define DFF_VERSION_GTAVC_1 3074
#define DFF_VERSION_GTAVC_2 4099
#define DFF_VERSION_GTASA 6147



/**	\brief Internal structure used for the header of the DFF geometry struct section.
 */
struct DFFGeometryStructHeader {
	int16_t flags;
	uint8_t uvSetCount;
	uint8_t _unknown;
	int32_t faceCount;
	int32_t vertexCount;
	int32_t frameCount;
};


/**	\brief Internal structure which stores the current loading state of a DFFLoader.
 */
struct DFFLoadContext {
	DFFMesh* mesh;
	int32_t version;
	int depth;
	map<int32_t, DFFFrame*> frameInternalIndexMap;
	int32_t frameCurrentIndex;
};



/**	\brief A loader class for GTA DFF files.
 *
 * 	DFF is the mesh file format of the GTAIII trilogy, with origins in the RenderWare engine.
 * 	This class supports loading these binary files. DFF is a quite complex and still not fully documented
 * 	file format and this class does not attempt to load all data available in DFF files. DFF is a streaming
 * 	file based on <i>sections</i>, which themselves can contain data or child sections. Every section has an
 * 	ID stating it's type, and only some of the known structures are parsed by this class, all other sections
 * 	are skipped and ignored.
 * 	Basically, if loading succeeds, this class wil return a DFFMesh object, an abstract data structure
 * 	containing the important mesh data.
 *
 * 	@see DFFMesh
 */
class DFFLoader {
public:
	/**	\brief Creates a new DFFLoader.
	 */
	DFFLoader();

	/**	\brief Destructor.
	 *
	 * 	This does not affect any DFFMeshes loaded by this object.
	 */
	virtual ~DFFLoader();

	/**	\brief Loads a DFF mesh from the given stream.
	 *
	 *	If errors are found during loading, exceptions may be thrown to report them.
	 *
	 *	@param stream The stream to read the DFF mesh data from.
	 *	@return The loaded DFFMesh.
	 */
	DFFMesh* loadMesh(InputStream* stream);

	/**	\brief Loads a DFF mesh from the given file.
	 *
	 * 	@param file The file to load.
	 * 	@return The loaded DFFMesh.
	 * 	@see loadMesh(InputStream*)
	 */
	DFFMesh* loadMesh(const File& file);

	/**	\brief Sets the option for verbose mode.
	 *
	 * 	In verbose mode, while loading the mesh, the loader will write some debug information about key
	 * 	data it has loaded to standard out.
	 *
	 *	@param Whether the loader should be verbose.
	 */
	void setVerbose(bool verbose) { verboseMutex.lock(); this->verbose = verbose; verboseMutex.unlock(); }

	/**	\brief Returns whether this loader is in verbose mode.
	 *
	 * 	@return True if it's verbose.
	 * 	@see setVerbose()
	 */
	bool isVerbose() { return verbose; }

public:


protected:


private:
	int parseSection(InputStream* stream, RwSectionHeader* parent, DFFLoadContext* context);
	int parseStruct(InputStream* stream, RwSectionHeader& structHeader, RwSectionHeader* parent,
			DFFLoadContext* context);
	int parseString(InputStream* stream, RwSectionHeader& stringHeader, RwSectionHeader* parent,
			DFFLoadContext* context);
	int parseFrame(InputStream* stream, RwSectionHeader& frameHeader, RwSectionHeader* parent,
			DFFLoadContext* context);
	int parseMaterialSplit(InputStream* stream, RwSectionHeader& matsplitHeader, RwSectionHeader* parent,
			DFFLoadContext* context);

	void printHeaderInfo(DFFGeometryStructHeader& header, DFFLoadContext* context);

	/*inline int readSectionHeaderWithID(InputStream* stream, RwSectionHeader& header, uint32_t id)
	{
		RwReadSectionHeader(stream, header);

		if (header.id != id) {
			char expected[64];
			char found[64];
			RwGetSectionName(id, expected);
			RwGetSectionName(header.id, found);
			char errmsg[256];
			sprintf(errmsg, "Error: Found section with type %s where %s was expected!\n", found, expected);
			throw DFFException(errmsg, __FILE__, __LINE__);
		}

		return sizeof(header);
	}
	inline int skipSectionHeaderWithID(InputStream* stream, uint32_t id)
	{
		RwSectionHeader header;
		readSectionHeaderWithID(stream, header, id);
		return sizeof(header);
	}
	inline int skipSectionWithID(InputStream* stream, uint32_t id)
	{
		RwSectionHeader header;
		readSectionHeaderWithID(stream, header, id);
		return RwSkipSectionBody(stream, header) + sizeof(header);
	}*/

private:
	bool verbose;
	Mutex verboseMutex;
};

#endif /* DFFLOADER_H_ */
