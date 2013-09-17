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

#include "IFPLoader.h"
#include "IFPUnknownFrame2.h"
#include "IFPException.h"



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

	if (strncmp(fourCC, "ANP3", 4)  ==  0) {
		ver = ANP3;
	} else if (strncmp(fourCC, "ANPK", 4) == 0) {
		ver = ANPK;
	} else {
		throw IFPException("Unsupported file format (not ANP3)!", __FILE__, __LINE__);
	}

	stream->read((char*) &endOffs, sizeof(int32_t));

	if (ver == ANPK) {
		stream->ignore(8); // INFO header

		stream->read((char*) &numAnims, sizeof(int32_t));

		ifpName = readVER1String();
	} else {
		char* name = new char[25];
		name[24] = '\0';
		stream->read(name, 24);
		ifpName = CString::from(name);

		stream->read((char*) &numAnims, sizeof(int32_t));
	}
}


IFPAnimation* IFPLoader::readAnimation()
{
	if (stream->tellg() >= (std::streamoff) (endOffs+8))
		return NULL;

	IFPAnimation* anim = new IFPAnimation;

	if (ver == ANPK) {
		// NAME section start

		stream->ignore(4);

		int32_t nameEnd;
		stream->read((char*) &nameEnd, 4);

		//int32_t nameLen = nameEnd + (4 - nameEnd%4);
		int32_t nameLen = (nameEnd + 3) & ~0x03;
		char* aname = new char[nameLen];
		stream->read(aname, nameLen);
		anim->name = CString::from(aname);

		char dgan[4];
		stream->read(dgan, 4);

		// DGAN section start

		stream->ignore(12); // DGAN + INFO header

		int32_t numObjs;
		stream->read((char*) &numObjs, sizeof(int32_t));

		skipVER1String();

		bool cpanNameRead = false;
		for (int32_t i = 0 ; i < numObjs ; i++) {
			IFPObject* obj = new IFPObject;

			obj->boneID = -1;

			// CPAN section start
			if (!cpanNameRead) {
				stream->ignore(12);
			} else {
				stream->ignore(8);
				cpanNameRead = false;
			}

			int32_t animLen;
			stream->read((char*) &animLen, sizeof(int32_t));

			// ANIM section data start

			char* objName = new char[28];
			stream->read(objName, 28);
			obj->name = CString::from(objName);

			int32_t numFrames;
			stream->read((char*) &numFrames, sizeof(int32_t));

			stream->ignore(12); // unknown (usually 0), next sibling, previous sibling

			// Sometimes, there seems to be some junk in ANIM. We'll just ignore this
			stream->ignore(animLen - 28 - 16);

			// KFRM section start

			char kfrmType[4];
			stream->read(kfrmType, 4);

			// If numFrames = 0, then the KFRM data section may be omitted completely
			if (kfrmType[0] != 'C'  ||  kfrmType[1] != 'P'  ||  kfrmType[2] != 'A'  ||  kfrmType[3] != 'N') {
				stream->ignore(4);

				if (kfrmType[2] == '0') {
					// KR00

					obj->frameType = IFPObject::RotFrame;

					for (int32_t j = 0 ; j < numFrames ; j++) {
						IFPRotFrame* frame = new IFPRotFrame;

						stream->read(((char*) &frame->rot)+4, sizeof(Quaternion)-4);
						stream->read((char*) &frame->rot, 4);
						frame->setRotation(frame->rot.conjugate());
						//frame->setRotation(Quaternion(frame->rot.getW(), frame->rot.getX(), frame->rot.getY(), frame->rot.getZ()));
						stream->read((char*) &frame->time, sizeof(float));

						obj->frames.push_back(frame);
					}
				} else if (kfrmType[3] == '0') {
					// KRT0

					obj->frameType = IFPObject::RotTransFrame;

					for (int32_t j = 0 ; j < numFrames ; j++) {
						IFPRotTransFrame* frame = new IFPRotTransFrame;

						stream->read(((char*) &frame->rot)+4, sizeof(Quaternion)-4);
						stream->read((char*) &frame->rot, 4);
						frame->setRotation(frame->rot.conjugate());
						stream->read((char*) &frame->trans, sizeof(Vector3));
						stream->read((char*) &frame->time, sizeof(float));

						obj->frames.push_back(frame);
					}
				} else {
					// KRTS

					obj->frameType = IFPObject::RotTransScaleFrame;

					for (int32_t j = 0 ; j < numFrames ; j++) {
						IFPRotTransScaleFrame* frame = new IFPRotTransScaleFrame;

						stream->read(((char*) &frame->rot)+4, sizeof(Quaternion)-4);
						stream->read((char*) &frame->rot, 4);
						frame->setRotation(frame->rot.conjugate());
						stream->read((char*) &frame->trans, sizeof(Vector3));
						stream->read((char*) &frame->scale, sizeof(Vector3));
						stream->read((char*) &frame->time, sizeof(float));

						obj->frames.push_back(frame);
					}
				}
			} else {
				if (numFrames != 0) {
					char errmsg[256];
					sprintf(errmsg, "Frame information missing although frame count != 0 for object '%s' "
							"(frame count: %d)",
							objName, numFrames);
					throw IFPException(errmsg, __FILE__, __LINE__);
				}

				cpanNameRead = true;
			}

			anim->addObject(obj);
		}
	} else {
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
				obj->frameType = IFPObject::RotFrame;
				break;
			case 4:
				obj->frameType = IFPObject::RotTransFrame;
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
					IFPRotTransFrame* frame = new IFPRotTransFrame;

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
					IFPRotFrame* frame = new IFPRotFrame;

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
	}

	return anim;
}
