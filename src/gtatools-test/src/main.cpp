/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include <gta/gl.h>
#include <cstdio>
#include <fstream>
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/util.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/gtaide.h>
#include <gtaformats/gtaipl.h>
#include <gtaformats/gtacol.h>
#include <gtaformats/gtatxd.h>
#include <cmath>
#include <gta/Camera.h>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <vector>
#include <queue>
#include <gta/Engine.h>
#include <gtatools-test/config.h>
#include <istream>
#include <utility>
#include <climits>
#include <set>
#include <gta/ItemManager.h>
#include <gta/resource/ResourceCache.h>
#include <gta/scene/Scene.h>
#include <iostream>
#include <gta/GLException.h>
#include "Controller.h"
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_syswm.h>

using std::ifstream;
using std::vector;
using std::queue;
using std::istream;
using std::max;
using std::min;
using std::set;
using std::streamoff;
using std::cerr;
using std::endl;
using std::dec;
using std::hex;
using std::ofstream;


#ifdef GT_USE_OPENGL_ES
EGLSurface eglSurface;
EGLDisplay eglDisplay;
EGLContext eglContext;
bool firstInitialization = true;
#endif

SDL_Surface* surface;



void initWindowSystem(int w, int h)
{
	Uint32 videoMode = SDL_RESIZABLE;

#ifdef GT_WINDOW_FULLSCREEN
	videoMode |= SDL_FULLSCREEN;
#endif

#ifdef GT_USE_OPENGL_ES
	SDL_Surface* screen = SDL_SetVideoMode(w, h, 16, videoMode);
	surface = screen;

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	if (!SDL_GetWMInfo(&wmInfo)) {
		printf("Error\n");
		exit(-1);
	}

	eglDisplay = eglGetDisplay((EGLNativeDisplayType) wmInfo.info.x11.display);

	if (eglDisplay == EGL_NO_DISPLAY) {
		cerr << "ERROR: No display available!" << endl;
		return;
	}

	EGLint major, minor;

	if (!eglInitialize(eglDisplay, &major, &minor)) {
		cerr << "ERROR: Could not initialize EGL (error code 0x" << hex << eglGetError() << dec << ")!" << endl;
		return;
	}

	EGLint eglAttribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_RED_SIZE, 5,
			EGL_GREEN_SIZE, 5,
			EGL_BLUE_SIZE, 5,
			EGL_DEPTH_SIZE, 16,
			EGL_NONE
	};

	EGLConfig config;
	EGLint numConfigs;

	if (!eglChooseConfig(eglDisplay, eglAttribs, &config, 1, &numConfigs)) {
		cerr << "ERROR: Could not choose config (error code 0x" << hex << eglGetError() << dec << ")!" << endl;
		return;
	}

	if (numConfigs < 1) {
		cerr << "ERROR: no matching config could be found (error code 0x" << hex << eglGetError() << dec << ")!" << endl;
		return;
	}

	EGLint redSize, greenSize, blueSize, depthSize;

	eglGetConfigAttrib(eglDisplay, config, EGL_RED_SIZE, &redSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_GREEN_SIZE, &greenSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_BLUE_SIZE, &blueSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_DEPTH_SIZE, &depthSize);

	printf("Red: %d bits\n", redSize);
	printf("Green: %d bits\n", greenSize);
	printf("Blue: %d bits\n", blueSize);
	printf("Depth: %d bits\n", depthSize);

	eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType) wmInfo.info.x11.window, NULL);

	if (eglSurface == EGL_NO_SURFACE) {
		cerr << "ERROR: Creating surface failed (error code 0x" << hex << eglGetError() << dec << ")!" << endl;
		return;
	}

	if (firstInitialization) {
		EGLint eglContextAttribs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 2,
				EGL_NONE
		};

		eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, eglContextAttribs);

		if (eglContext == EGL_NO_CONTEXT) {
			cerr << "ERROR: Creating GLES2 context failed (error code 0x" << hex << eglGetError() << dec << ")!" << endl;
			return;
		}
	}

	if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
		cerr << "ERROR making GLES2 context current (error code 0x" << hex << eglGetError() << dec << ")!" << endl;
		return;
	}

	firstInitialization = false;
#else
	const SDL_VideoInfo* info = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//surface = SDL_SetVideoMode(w, h, info->vfmt->BitsPerPixel, videoMode | SDL_OPENGL);
	surface = SDL_SetVideoMode(w, h, info->vfmt->BitsPerPixel, SDL_HWSURFACE | SDL_OPENGL);

	int dsize;
	int r, g, b, a;
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &dsize);
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a);
	printf("Framebuffer: R%d G%d B%d A%d D%d\n", r, g, b, a, dsize);
#endif
}



int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR Initializing SDL\n");
	}

	initWindowSystem(WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_WM_SetCaption(WINDOW_BASE_TITLE, NULL);

	Controller renderer;

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	renderer.init();
	renderer.reshape(WINDOW_WIDTH, WINDOW_HEIGHT);

	bool running = true;
	while (running) {
		if (!renderer.paint()) {
			break;
		}

#ifdef GT_USE_OPENGL_ES
		eglSwapBuffers(eglDisplay, eglSurface);
#else
		SDL_GL_SwapBuffers();
#endif

		SDL_Event evt;
		while (SDL_PollEvent(&evt) != 0) {
			switch (evt.type) {
			case SDL_KEYDOWN:
				if (evt.key.keysym.sym == SDLK_q  &&  (evt.key.keysym.mod & KMOD_CTRL) != 0) {
					running = false;
					break;
				}

				renderer.keyPressed(evt.key.keysym);
				break;
			case SDL_KEYUP:
				renderer.keyReleased(evt.key.keysym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				renderer.mouseButtonPressed(evt.button.button, evt.button.x, evt.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				renderer.mouseButtonReleased(evt.button.button, evt.button.x, evt.button.y);
				break;
			case SDL_MOUSEMOTION:
				renderer.mouseMotion(evt.motion.x, evt.motion.y);
				break;
			case SDL_QUIT:
				running = false;
				break;
			case SDL_VIDEORESIZE:
#ifdef GT_USE_OPENGL_ES
				//eglDestroyContext(eglDisplay, eglContext);
				eglDestroySurface(eglDisplay, eglSurface);
#endif
				SDL_FreeSurface(surface);
				initWindowSystem(evt.resize.w, evt.resize.h);
				renderer.reshape(evt.resize.w, evt.resize.h);
				break;
			}
		}
	}

	SDL_Quit();

	return 0;
}

