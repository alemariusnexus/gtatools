/*
 * WildcardTextureFilter.cpp
 *
 *  Created on: 28.12.2010
 *      Author: alemariusnexus
 */

#include "WildcardTextureFilter.h"
#include <gtaformats/util/strutil.h>
#include <cstring>



WildcardTextureFilter::WildcardTextureFilter(const char* pattern)
		: pattern(new char[strlen(pattern)])
{
	strcpy(this->pattern, pattern);
}


bool WildcardTextureFilter::process(const TXDTexture& tex)
{
	return WildcardMatch(pattern, tex.getDiffuseName())  ||  WildcardMatch(pattern, tex.getAlphaName());
}
