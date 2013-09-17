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

#include "image.h"
#include <cmath>



float ComputeMeanSquaredError(uint8_t* data1, uint8_t* data2, uint16_t w, uint16_t h, bool alpha)
{
	uint64_t errorSum = 0;

	int bpp = alpha ? 4 : 3;

	for (uint16_t i = 0 ; i < h ; i++) {
		for (uint16_t j = 0 ; j < w ; j++) {
			int16_t rError = data2[(i*w + j) * bpp] - data1[(i*w + j) * bpp];
			int16_t gError = data2[(i*w + j) * bpp + 1] - data1[(i*w + j) * bpp + 1];
			int16_t bError = data2[(i*w + j) * bpp + 2] - data1[(i*w + j) * bpp + 2];
			int16_t aError;

			if (alpha)
				aError = data2[(i*w + j) * bpp + 3] - data1[(i*w + j) * bpp + 3];
			else
				aError = 0;

			errorSum += rError*rError + gError*gError + bError*bError + aError*aError;
		}
	}

	float mse = errorSum / (float) (w*h*bpp);
	return mse;
}


float ComputePeakSignalToNoiseRatio(bool& equal, uint8_t* data1, uint8_t* data2, uint16_t w, uint16_t h,
		bool alpha)
{
	float mse = ComputeMeanSquaredError(data1, data2, w, h, alpha);

	if (mse != 0.0f) {
		equal = false;
		return 10.0f * log10(255*255 / mse);
	} else {
		equal = true;
		return 0.0f;
	}
}
