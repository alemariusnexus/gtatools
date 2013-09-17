/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef RWBS_H_
#define RWBS_H_

#include <gtaformats/config.h>
#include <cstring>
#include <cstdio>
#include <map>

using std::map;


#define RW_SECTION_INVALID 0
#define RW_SECTION_STRUCT 1				// Alias Data
#define RW_SECTION_STRING 2
#define RW_SECTION_EXTENSION 3
#define RW_SECTION_CAMERA 5
#define RW_SECTION_TEXTURE 6
#define RW_SECTION_MATERIAL 7
#define RW_SECTION_MATERIALLIST 8
#define RW_SECTION_FRAMELIST 14
#define RW_SECTION_GEOMETRY 15
#define RW_SECTION_CLUMP 16
#define RW_SECTION_ATOMIC 20
#define RW_SECTION_GEOMETRYLIST 26
#define RW_SECTION_MATERIALSPLIT 1294	// Alias Bin Mesh PLG
#define RW_SECTION_FRAME 39056126

#define RW_SECTION_SKIN_PLG 0x0116
#define RW_SECTION_HANIM_PLG 0x011E
#define RW_SECTION_SKY_MIPMAP_VAL 0x0110
#define RW_SECTION_MATERIAL_EFFECTS_PLG 0x0120
#define RW_SECTION_SPECULAR_MATERIAL 0x0253F2F6
#define RW_SECTION_NIGHT_VERTEX_COLORS 0x0253F2F9
#define RW_SECTION_REFLECTION_MATERIAL 0x0253F2FC
#define RW_SECTION_2DFX 0x0253F2F8
#define RW_SECTION_MESH_EXTENSION 0x0253F2FD
#define RW_SECTION_RIGHT_TO_RENDER 0x001F
#define RW_SECTION_COLLISION_MODEL 0x0253F2FA

#define RW_SECTION_TEXTUREDICTIONARY 0x16
#define RW_SECTION_TEXTURENATIVE 0x15

#define RW_SECTION_UV_ANIM_DICTIONARY 0x2B
#define RW_SECTION_UV_ANIM_PLG 0x135
#define RW_SECTION_MORPH_PLG 0x105
#define RW_SECTION_PIPELINE_SET 0x253F2F3

struct RWSectionInfo
{
	int32_t id;
	const char* name;
	bool container;
};


#define RW_NUM_KNOWN_SECTIONS (sizeof(RWKnownSections) / sizeof(RWSectionInfo))

static RWSectionInfo RWKnownSections[] = {
		{ RW_SECTION_STRUCT, "STRUCT", false },
		{ RW_SECTION_STRING, "STRING", false },
		{ RW_SECTION_EXTENSION, "EXTENSION", true },
		{ RW_SECTION_CAMERA, "CAMERA", false },
		{ RW_SECTION_TEXTURE, "TEXTURE", true },
		{ RW_SECTION_MATERIAL, "MATERIAL", true },
		{ RW_SECTION_MATERIALLIST, "MATERIALLIST", true },
		{ RW_SECTION_FRAMELIST, "FRAMELIST", true },
		{ RW_SECTION_GEOMETRY, "GEOMETRY", true },
		{ RW_SECTION_CLUMP, "CLUMP", true },
		{ RW_SECTION_ATOMIC, "ATOMIC", true },
		{ RW_SECTION_GEOMETRYLIST, "GEOMETRYLIST", true },
		{ RW_SECTION_MATERIALSPLIT, "MATERIALSPLIT", false },
		{ RW_SECTION_FRAME, "FRAME", false },
		{ RW_SECTION_HANIM_PLG, "HANIM_PLG", false },
		{ RW_SECTION_SKY_MIPMAP_VAL, "SKY_MIPMAP_VAL", false },
		{ RW_SECTION_MATERIAL_EFFECTS_PLG, "MATERIAL_EFFECTS_PLG", true },
		{ RW_SECTION_SPECULAR_MATERIAL, "SPECULAR_MATERIAL", false },
		{ RW_SECTION_NIGHT_VERTEX_COLORS, "NIGHT_VERTEX_COLORS", false },
		{ RW_SECTION_REFLECTION_MATERIAL, "REFLECTION_MATERIAL", false },
		{ RW_SECTION_2DFX, "2DFX", false },
		{ RW_SECTION_MESH_EXTENSION, "MESH_EXTENSION", false },
		{ RW_SECTION_RIGHT_TO_RENDER, "RIGHT_TO_RENDER", false },
		{ RW_SECTION_COLLISION_MODEL, "COLLISION_MODEL", false },
		{ RW_SECTION_TEXTUREDICTIONARY, "TEXTUREDICTIONARY", true },
		{ RW_SECTION_TEXTURENATIVE, "TEXTURENATIVE", true },
		{ RW_SECTION_UV_ANIM_DICTIONARY, "UV_ANIM_DICTIONARY", true },
		{ RW_SECTION_UV_ANIM_PLG, "UV_ANIM_PLG", true },
		{ RW_SECTION_MORPH_PLG, "MORPH_PLG", false },
		{ RW_SECTION_PIPELINE_SET, "PIPELINE_SET", false }
};



#define RW_MAX_SECTION_NAME 64



bool RWGetSectionInfo(int32_t id, RWSectionInfo* info);


bool RWGetSectionName(int32_t id, char* dest);


inline bool RWGetSectionShortName(int32_t id, char* dest)
{
	RWSectionInfo info;
	if (RWGetSectionInfo(id, &info)) {
		strcpy(dest, info.name);
		return true;
	} else {
		sprintf(dest, "[Unknown: 0x%X]", id);
		return false;
	}
}


inline bool RWIsSectionContainer(int32_t id)
		{ RWSectionInfo info; RWGetSectionInfo(id, &info); return info.container; }


inline bool RWIsSectionKnown(int32_t id) { RWSectionInfo info; return RWGetSectionInfo(id, &info); }

#endif /* RWBS_H_ */
