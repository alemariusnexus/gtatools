/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef SCENEOBJECTGROUPDEPENDENCY_H_
#define SCENEOBJECTGROUPDEPENDENCY_H_

#include <gta/config.h>
#include <gtaformats/util/CString.h>
#include <gtaformats/util/CRC32.h>


class SceneObjectGroupDependency {
public:
	SceneObjectGroupDependency(const CString& relPath);
	CString getRelativePath() const { return relPath; }
	void setChecksum(uint32_t cs) { checksum = cs; }
	uint32_t getChecksum() const { return checksum; }

private:
	CString relPath;
	uint32_t checksum;
};

#endif /* SCENEOBJECTGROUPDEPENDENCY_H_ */