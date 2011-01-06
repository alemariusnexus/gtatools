/*
 * WildcardTextureFilter.h
 *
 *  Created on: 28.12.2010
 *      Author: alemariusnexus
 */

#ifndef WILDCARDTEXTUREFILTER_H_
#define WILDCARDTEXTUREFILTER_H_

#include "TextureFilter.h"


class WildcardTextureFilter : public TextureFilter {
public:
	WildcardTextureFilter(const char* pattern);
	virtual bool process(const TXDTexture& tex);

private:
	char* pattern;
};

#endif /* WILDCARDTEXTUREFILTER_H_ */
