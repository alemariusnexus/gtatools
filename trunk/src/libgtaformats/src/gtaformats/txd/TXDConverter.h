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

#ifndef TXDCONVERTER_H_
#define TXDCONVERTER_H_

#include <gtaformats/config.h>
#include "TXDTextureHeader.h"


class TXDConverter {
public:
	enum CompressionHint
	{
		CompressionHintFast,
		CompressionHintSlow,
		CompressionHintVerySlow
	};

public:
	TXDConverter();
	int32_t convert(const TXDTextureHeader& from, const TXDTextureHeader& to, const uint8_t* fromData,
			uint8_t* toData, int8_t mipStart, int8_t mipEnd);
	uint8_t* convert(const TXDTextureHeader& from, const TXDTextureHeader& to, const uint8_t* fromData,
			int8_t mipStart, int8_t mipEnd);
	bool canConvert(const TXDTextureHeader& from, const TXDTextureHeader& to);
	void setCompressionHint(CompressionHint hint) { comprHint = hint; }

private:
	void getFormatSpecifics(int32_t rasterFormat, int32_t& rm, int32_t& gm, int32_t& bm, int32_t& am,
			int32_t& rs, int32_t& gs, int32_t& bs, int32_t& as, uint8_t& rMax, uint8_t& gMax, uint8_t& bMax,
			uint8_t& aMax, int8_t& bpp, bool& swapBytes);

private:
	CompressionHint comprHint;
};

#endif /* TXDCONVERTER_H_ */
