/*
 * gldebug.h
 *
 *  Created on: 16.10.2011
 *      Author: alemariusnexus
 */

#ifndef GLDEBUG_H_
#define GLDEBUG_H_

#include <gta/config.h>
#include "gl.h"
#include <gtaformats/util/File.h>
#include <ostream>

using std::ostream;

enum PixelSaveFormat
{
	PixelSaveFormatRGBA,
	PixelSaveFormatRGB
};

void GLDebugInit();

void GLDebugSavePixels(ostream* out, int x, int y, int w, int h, PixelSaveFormat format = PixelSaveFormatRGB);
void GLDebugSavePixels(ostream* out, PixelSaveFormat format = PixelSaveFormatRGB);
void GLDebugSavePixels(const File& file, int x, int y, int w, int h, PixelSaveFormat format = PixelSaveFormatRGB);
void GLDebugSavePixels(const File& file, PixelSaveFormat format = PixelSaveFormatRGB);

void GLDebugReadDepthTexture(float* buf, GLenum target = GL_TEXTURE_2D);
float* GLDebugReadDepthTexture(GLenum target = GL_TEXTURE_2D);
void GLDebugSaveDepthTexture(ostream* out, GLenum target = GL_TEXTURE_2D);
void GLDebugSaveDepthTexture(const File& file, GLenum target = GL_TEXTURE_2D);

#endif /* GLDEBUG_H_ */
