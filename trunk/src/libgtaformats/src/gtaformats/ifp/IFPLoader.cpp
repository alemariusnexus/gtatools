/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "IFPLoader.h"
#include "IFPUnknownFrame2.h"



IFPLoader::IFPLoader(istream* stream)
		: stream(stream)
{
	init();
}


IFPLoader::IFPLoader(const File& file)
		: stream(file.openInputStream(istream::binary))
{
	init();
}


IFPLoader::~IFPLoader()
{
	delete stream;
}


void IFPLoader::init()
{
	char fourCC[4];
	stream->read(fourCC, 4);

	stream->read((char*) &endOffs, sizeof(int32_t));

	char* name = new char[25];
	name[24] = '\0';
	stream->read(name, 24);
	ifpName = CString::from(name);

	stream->read((char*) &numAnims, sizeof(int32_t));
}


IFPAnimation* IFPLoader::readAnimation()
{
	if (stream->tellg() >= (std::streamoff) (endOffs+8))
		return NULL;

	IFPAnimation* anim = new IFPAnimation;

	char* aname = new char[25];
	aname[24] = '\0';
	stream->read(aname, 24);
	anim->name = CString::from(aname);

	int32_t numObjs;
	stream->read((char*) &numObjs, sizeof(int32_t));

	stream->ignore(2 * sizeof(int32_t)); // frame data size, unknown

	anim->objs.reserve(numObjs);

	for (int32_t j = 0 ; j < numObjs ; j++) {
		IFPObject* obj = new IFPObject;

		char* oname = new char[25];
		oname[24] = '\0';
		stream->read(oname, 24);
		obj->name = CString::from(oname).trim();

		int32_t frameType;
		stream->read((char*) &frameType, sizeof(int32_t));
		//obj->frameType = (frameType == 4) ? IFPObject::RootFrame : IFPObject::ChildFrame;

		switch (frameType) {
		case 2:
			obj->frameType = IFPObject::Unknown2Frame;
			break;
		case 3:
			obj->frameType = IFPObject::ChildFrame;
			break;
		case 4:
			obj->frameType = IFPObject::RootFrame;
			break;
		}

		int32_t numFrames;
		stream->read((char*) &numFrames, sizeof(int32_t));

		int32_t boneID;
		stream->read((char*) &obj->boneID, sizeof(int32_t));

		obj->frames.reserve(numFrames);

		if (frameType == 4) {
			// Root frames

			for (int32_t k = 0 ; k < numFrames ; k++) {
				IFPRootFrame* frame = new IFPRootFrame;

				int16_t cdata[8];

				stream->read((char*) cdata, 8*sizeof(int16_t));

				frame->rot = Quaternion(cdata[3] / 4096.0f, cdata[0] / 4096.0f, cdata[1] / 4096.0f,
						cdata[2] / 4096.0f);
				frame->time = cdata[4] / 60.0f;
				frame->trans = Vector3(cdata[5] / 1024.0f, cdata[6] / 1024.0f, cdata[7] / 1024.0f);

				obj->frames.push_back(frame);
			}
		} else if (frameType == 3) {
			// Child frames

			for (int32_t k = 0 ; k < numFrames ; k++) {
				IFPFrame* frame = new IFPFrame;

				int16_t cdata[5];

				stream->read((char*) cdata, 5*sizeof(int16_t));

				frame->rot = Quaternion(cdata[3] / 4096.0f, cdata[0] / 4096.0f, cdata[1] / 4096.0f,
						cdata[2] / 4096.0f);
				frame->time = cdata[4] / 60.0f;

				obj->frames.push_back(frame);
			}
		} else if (frameType == 2) {
			// Unknown, 32 bytes per frame

			stream->ignore(numFrames*32);

			for (int32_t k = 0 ; k < numFrames ; k++) {
				IFPUnknownFrame2* frame = new IFPUnknownFrame2;
				frame->rot = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
				frame->time = 0.0f;
				obj->frames.push_back(frame);
			}
		}

		anim->objs.push_back(obj);
	}

	return anim;
}
