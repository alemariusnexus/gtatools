/* 
 * File:   gta.h
 * Author: alemariusnexus
 *
 * Created on 15. November 2009, 21:56
 */

#ifndef _GTA_H
#define	_GTA_H

#include "config.h"
#include <istream>


using std::istream;



#define RW_VERSION_GTA3_1 0x0800FFFF
#define RW_VERSION_GTA3_2 0x00000310
#define RW_VERSION_GTAVC_1 0x0C02FFFF
#define RW_VERSION_GTAVC_2 0x1003FFFF
#define RW_VERSION_GTASA 0x1803FFFF

#define RW_VERSION_NR_GTA3_1 "3.?.?.?"
#define RW_VERSION_NR_GTA3_2 "3.1.0.0"
#define RW_VERSION_NR_GTAVC_1 "3.3.0.2"
#define RW_VERSION_NR_GTAVC_2 "3.4.0.3"
#define RW_VERSION_NR_GTASA "3.6.0.3"

#define RW_SECTION_INVALID 0
#define RW_SECTION_STRUCT 1				// Alias Data
#define RW_SECTION_STRING 2
#define RW_SECTION_EXTENSION 3
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


struct RwSectionHeader {
	uint32_t id;
	int32_t size;
	int32_t version;
};

//#define RwReadSectionHeader(s,h) (s)->read(&(h), sizeof(RwSectionHeader))

inline int RwReadSectionHeader(istream* stream, RwSectionHeader& header) {
	stream->read((char*) &header, sizeof(RwSectionHeader));
	return sizeof(RwSectionHeader);
}

inline int RwSkipSectionBody(istream* stream, RwSectionHeader& header) {
	char* skipBuffer = new char[header.size];
	stream->read(skipBuffer, header.size);
	return header.size;
}


void RwGetSectionName(uint32_t id, char* dest);
void RwGetVersionName(uint32_t version, char* dest);
//int RwReadSectionHeaderWithID(istream* stream, RwSectionHeader& header, uint32_t id);
//int RwSkipSectionHeaderWithID(istream* stream, uint32_t id);
//int RwSkipSectionWithID(istream* stream, uint32_t id);

int Crc32(const char value[]);

#endif	/* _GTA_H */

