/*
 * NullTextureFilter.h
 *
 *  Created on: 28.12.2010
 *      Author: alemariusnexus
 */

#ifndef NULLTEXTUREFILTER_H_
#define NULLTEXTUREFILTER_H_

#include "TextureFilter.h"


class NullTextureFilter : public TextureFilter {
public:
	virtual bool process(const TXDTextureHeader& tex) { return true; }
};

#endif /* NULLTEXTUREFILTER_H_ */
