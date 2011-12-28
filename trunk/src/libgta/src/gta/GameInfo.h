/*
 * GameInfo.h
 *
 *  Created on: 22.10.2011
 *      Author: alemariusnexus
 */

#ifndef GAMEINFO_H_
#define GAMEINFO_H_

#include <gtaformats/util/File.h>



class GameInfo {
public:
	enum VersionMode
	{
		GTAIII,
		GTAVC,
		GTASA
	};

public:
	GameInfo(VersionMode ver, const File& rootDir);
	GameInfo(const GameInfo& other) : ver(other.ver), rootDir(other.rootDir) {}
	VersionMode getVersionMode() const { return ver; }
	File getRootDirectory() const { return rootDir; }

private:
	VersionMode ver;
	File rootDir;
};

#endif /* GAMEINFO_H_ */
