/*
 * DFFXmlConverter.h
 *
 *  Created on: 28.02.2010
 *      Author: alemariusnexus
 */

#ifndef DFFXMLCONVERTER_H_
#define DFFXMLCONVERTER_H_

#include "DFFMesh.h"
#include "../tinyxml/tinyxml.h"
#include <iostream>

using std::ostream;


class DFFXmlConverter
{
public:
	TiXmlDocument* convert(DFFMesh* mesh);
	bool save(DFFMesh* mesh, const char* filename);
};

#endif /* DFFXMLCONVERTER_H_ */
