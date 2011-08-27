/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	ifpName[24] = '\0';
	stream->read(ifpName, 24);

	stream->read((char*) &numAnims, sizeof(int32_t));
}


IFPAnimation* IFPLoader::readAnimation()
{
	if (stream->tellg() >= (std::streamoff) (endOffs+8))
		return NULL;

	IFPAnimation* anim = new IFPAnimation;

	anim->name = new char[25];
	anim->name[24] = '\0';
	stream->read(anim->name, 24);

	int32_t numObjs;
	stream->read((char*) &numObjs, sizeof(int32_t));

	stream->ignore(2 * sizeof(int32_t)); // frame data size, unknown

	anim->objs.reserve(numObjs);

	for (int32_t j = 0 ; j < numObjs ; j++) {
		IFPObject* obj = new IFPObject;

		obj->name = new char[25];
		obj->name[24] = '\0';
		stream->read(obj->name, 24);

		int32_t frameType;
		stream->read((char*) &frameType, sizeof(int32_t));
		obj->frameType = (frameType == 3) ? IFPObject::ChildFrame : IFPObject::RootFrame;

		int32_t numFrames;
		stream->read((char*) &numFrames, sizeof(int32_t));

		int32_t boneID;
		stream->read((char*) &obj->boneID, sizeof(int32_t));

		obj->frames.reserve(numFrames);

		if (frameType == 3) {
			// Child frames

			for (int32_t k = 0 ; k < numFrames ; k++) {
				IFPFrame* frame = new IFPFrame;

				int16_t cdata[5];

				stream->read((char*) cdata, 5*sizeof(int16_t));

				frame->rot = Quaternion(cdata[0] / 4096.0f, cdata[1] / 4096.0f, cdata[2] / 4096.0f,
						cdata[3] / 4096.0f);
				frame->time = cdata[4] / 1024.0f;

				obj->frames.push_back(frame);
			}
		} else {
			// Root frames

			for (int32_t k = 0 ; k < numFrames ; k++) {
				IFPRootFrame* frame = new IFPRootFrame;

				int16_t cdata[8];

				stream->read((char*) cdata, 8*sizeof(int16_t));

				frame->rot = Quaternion(cdata[0] / 4096.0f, cdata[1] / 4096.0f, cdata[2] / 4096.0f,
						cdata[3] / 4096.0f);
				frame->time = cdata[4] / 1024.0f;
				frame->trans = Vector3(cdata[5] / 1024.0f, cdata[6] / 1024.0f, cdata[7] / 1024.0f);

				obj->frames.push_back(frame);
			}
		}

		anim->objs.push_back(obj);
	}

	return anim;
}
