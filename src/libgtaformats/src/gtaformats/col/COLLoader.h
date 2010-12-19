/*
 * COLLoader.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLLOADER_H_
#define COLLOADER_H_

#include "COLModel.h"
#include "../util/stream/InputStream.h"
#include "../util/File.h"



class COLLoader {
public:
	COLModel* loadModel(InputStream* stream);
	COLModel* loadModel(const File& file);
	bool loadModelName(InputStream* stream, char* name);
	bool loadModelName(const File& file, char* name);
	COLVersion getVersion(InputStream* stream);
	COLVersion getVersion(const File& file);

private:

};

#endif /* COLLOADER_H_ */
