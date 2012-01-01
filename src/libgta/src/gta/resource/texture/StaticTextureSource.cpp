/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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


#include "StaticTextureSource.h"



StaticTextureSource::StaticTextureSource(TXDArchive* txd)
{
	for (TXDArchive::TextureIterator it = txd->getHeaderBegin() ; it != txd->getHeaderEnd() ; it++) {
		TXDTextureHeader* txdTex = *it;
		Texture* tex = new Texture(txd, txdTex);

		texes[CString(txdTex->getDiffuseName()).lower()] = shared_ptr<Texture>(tex);
	}
}


StaticTextureSource::StaticTextureSource(const StaticTextureSource& other)
{
	for (TexMap::const_iterator it = other.texes.begin() ; it != other.texes.end() ; it++) {
		texes[it->first] = it->second;
	}
}


Texture* StaticTextureSource::getTexture(const CString& texHash)
{
	TexMap::iterator it = texes.find(texHash);

	if (it == texes.end())
		return NULL;

	return it->second.get();
}
