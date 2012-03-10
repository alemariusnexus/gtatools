/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtaformats-test.

	gtaformats-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <gtest/gtest.h>
#include <gtaformats/config.h>
#include <gtaformats/util/File.h>

enum GTATestVersion {
	TestGTAIII,
	TestGTAVC,
	TestGTASA
};


#ifdef GLOBAL_H_DEF
bool gta3Enabled = false;
bool gtavcEnabled = false;
bool gtasaEnabled = false;

File gta3Root = File("");
File gtavcRoot = File("");
File gtasaRoot = File("");

File testRootDir = File("");
#else
extern bool gta3Enabled;
extern bool gtavcEnabled;
extern bool gtasaEnabled;

extern File gta3Root;
extern File gtavcRoot;
extern File gtasaRoot;

extern File testRootDir;
#endif

#endif /* GLOBAL_H_ */
