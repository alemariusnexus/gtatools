/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef GAMEINFO_H_
#define GAMEINFO_H_

#include <nxcommon/file/File.h>



class GameInfo {
public:
	enum VersionMode
	{
		GTAIII,
		GTAVC,
		GTASA
	};

public:
	GameInfo() : valid(false), ver(GTASA), rootDir("") {}
	GameInfo(VersionMode ver, const File& rootDir);
	GameInfo(const GameInfo& other) : valid(other.valid), ver(other.ver), rootDir(other.rootDir) {}
	bool isValid() const { return valid; }
	VersionMode getVersionMode() const { return ver; }
	File getRootDirectory() const { return rootDir; }

private:
	bool valid;
	VersionMode ver;
	File rootDir;
};

#endif /* GAMEINFO_H_ */
