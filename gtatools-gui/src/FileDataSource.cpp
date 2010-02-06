/*
 * FileDataSource.cpp
 *
 *  Created on: 04.02.2010
 *      Author: alemariusnexus
 */

#include "FileDataSource.h"
#include <cstdio>


FileDataSource::FileDataSource(const wxString& filename)
		: DataSource(filename), filename(filename)
{
	//printf("NAME:%s\n", (const char*) getFilename().mb_str());
}

