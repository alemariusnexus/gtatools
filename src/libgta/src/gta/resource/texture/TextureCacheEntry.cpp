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

#include "TextureCacheEntry.h"
#include <utility>
#include "../../Engine.h"
#include <nxcommon/ResourceCache.h>
#include <functional>

using std::pair;



TextureCacheEntry::TextureCacheEntry(TextureArchive* archive)
#ifdef ADHOC_MAP
		: numTexes(0), size(0)
#else
		: size(0)
#endif
{
	if (archive->getParent()) {
		parentPtr = archive->getParent()->getCachePointer();
	}
}


TextureCacheEntry::~TextureCacheEntry()
{
#ifndef ADHOC_MAP
	TextureMap::iterator it;
	for (it = texMap.begin() ; it != texMap.end() ; it++) {
		delete it->second;
	}
#else
	for (size_t i = 0 ; i < numTexes ; i++) {
		delete texValues[i];
	}

	if (numTexes != 0)
		delete[] texValues;
#endif
}



void TextureCacheEntry::addTexture(const CString& name, Texture* tex)
{
#ifndef ADHOC_MAP
	texMap.insert(pair<CString, Texture*>(name, tex));
#else

	if (numTexes != 0) {
		hash_t* oldKeys = new hash_t[numTexes];
		memcpy(oldKeys, texKeys, numTexes*sizeof(hash_t));
		texKeys = new hash_t[numTexes+1];

		Texture** oldValues = new Texture*[numTexes];
		memcpy(oldValues, texValues, numTexes*sizeof(Texture*));
		texValues = new Texture*[numTexes+1];

		numTexes++;

		hash_t h = Hash(name.get());

		for (int i = 0 ; i < numTexes-1 ; i++) {
			if (h < oldKeys[i]) {
				memcpy(texKeys, oldKeys, i*sizeof(hash_t));
				texKeys[i] = h;
				memcpy(texKeys+i+1, oldKeys+i, (numTexes-1-i)*sizeof(hash_t));

				memcpy(texValues, oldValues, i*sizeof(Texture*));
				texValues[i] = tex;
				memcpy(texValues+i+1, oldValues+i, (numTexes-1-i)*sizeof(Texture*));

				break;
			}
		}

		if (h >= oldKeys[numTexes-2]) {
			memcpy(texKeys, oldKeys, (numTexes-1)*sizeof(hash_t));
			memcpy(texValues, oldValues, (numTexes-1)*sizeof(Texture*));

			texKeys[numTexes-1] = h;
			texValues[numTexes-1] = tex;
		}
	} else {
		texKeys = new hash_t[numTexes+1];
		texValues = new Texture*[numTexes+1];

		numTexes++;

		texKeys[numTexes-1] = Hash(name.get());
		texValues[numTexes-1] = tex;
	}

#endif

	this->size += tex->getSize();
}


// inclusive indices
//   0 <= imin when using truncate toward zero divide
//     imid = (imin+imax)/2;
//   imin unrestricted when using truncate toward minus infinity divide
//     imid = (imin+imax)>>1; or
//     imid = (int)floor((imin+imax)/2.0);
int binary_search(hash_t* a, hash_t key, int imin, int imax)
{
  // continually narrow search until just one element remains
  while (imin < imax)
    {
      //int imid = midpoint(imin, imax);
	  int imid = (imin+imax) / 2;

      // code must guarantee the interval is reduced at each iteration
      //assert(imid < imax);
      // note: 0 <= imin < imax implies imid will always be less than imax

      // reduce the search
      if (a[imid] < key)
        imin = imid + 1;
      else
        imax = imid;
    }
  // At exit of while:
  //   if A[] is empty, then imax < imin
  //   otherwise imax == imin

  // deferred test for equality
  if ((imax == imin) && (a[imin] == key))
    return imin;
  else
    return -1;
}


Texture* TextureCacheEntry::getTexture(const CString& texName)
{
#ifdef ADHOC_MAP
	hash_t hash = Hash(texName.get());

	/*hash_t* e = (hash_t*) bsearch(&hash, texKeys, numTexes, sizeof(hash_t), compareints);

	if (e == NULL)
		return NULL;

	return texValues[e - texKeys];*/

	int idx = binary_search(texKeys, hash, 0, numTexes-1);

	/*if (idx == -1)
		return NULL;*/

	if (idx == -1) {
		if (parentPtr.isValid()) {
			TextureCacheEntry* parentEntry = (TextureCacheEntry*) parentPtr.getEntry(true);
			return parentEntry->getTexture(texName);
		} else {
			return NULL;
		}
	}

	return texValues[idx];

	/*for (size_t i = 0 ; i < numTexes ; i++) {
		if (texKeys[i] == hash) {
			return texValues[i];
		}
	}*/

	return NULL;
#else

	TextureMap::const_iterator it = texMap.find(texName);

	if (it == texMap.end()) {
		/*if (parentPtr.isValid()) {
			TextureCacheEntry* entry = (TextureCacheEntry*) parentPtr.getEntry();

			if (entry) {
				return entry->getTexture(texName);
			}
		}*/
		return NULL;
	}

	return it->second;
	//return NULL;

#endif
}
