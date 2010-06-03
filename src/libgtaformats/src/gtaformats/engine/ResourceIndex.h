/*
 * ResourceIndex.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef RESOURCEINDEX_H_
#define RESOURCEINDEX_H_

#include "../gf_config.h"
#include "../util/StringComparator.h"
#include "../txd/TXDTexture.h"
#include "../txd/TXDArchive.h"
#include "../img/IMGArchive.h"
#include "../dff/DFFMesh.h"
#include <map>
#include <vector>

using std::map;
using std::vector;


struct TextureEntry
{
	const char* filename;
	int32_t imgIndex;
	int32_t txdIndex;
};

struct MeshEntry
{
	const char* filename;
	int32_t imgIndex;
};



class ResourceIndex {
public:
	enum RasterFormat {
		Native,
		R8G8B8A8
	};

private:
	typedef map<const char*, TextureEntry*, StringComparator> TextureMap;
	typedef map<const char*, MeshEntry*, StringComparator> MeshMap;

public:
	~ResourceIndex();
	void addResource(const char* filename);
	virtual bool getTextureHeader(const char* name, TXDTexture*& texture);
	virtual bool getTexture(const char* name, TXDTexture*& texture, uint8_t*& rawData);
	virtual bool getMesh(const char* name, DFFMesh*& mesh);

	void setTextureRasterFormat(RasterFormat format) { this->textureFormat = format; };

private:
	void indexTXD(TXDArchive* txd, const char* filename, int32_t imgIndex = -1);
	void indexDFF(const char* filename, const char* dffName, int32_t imgIndex = -1);
	bool gotoTexture(const char* name, TXDArchive*& txd, TXDTexture*& texture, IMGArchive*& img);

protected:
	RasterFormat textureFormat;

private:
	TextureMap textureIndex;
	MeshMap meshIndex;
};

#endif /* RESOURCEINDEX_H_ */
