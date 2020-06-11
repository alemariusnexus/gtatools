/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/config.cmake.h>
#include <nxcommon/math/Matrix4.h>
#include <nxcommon/util.h>
#include <nxcommon/strutil.h>
#include <nxcommon/math/intersection.h>
#include <nxcommon/math/project.h>
#include <nxcommon/file/DefaultFileFinder.h>
#include <nxcommon/file/FileException.h>
#include <nxcommon/log.h>
#include <gtaformats/config.h>
#include <gtaformats/gtadff.h>
#include <gtaformats/gtaide.h>
#include <gtaformats/gtaipl.h>
#include <gtaformats/gtacol.h>
#include <gtaformats/gtatxd.h>
#include <gtaformats/gtaimg.h>
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
#include <nxcommon/ResourceCache.h>
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
	//Uint32 videoMode = SDL_RESIZABLE;
	Uint32 videoMode = 0;

#ifdef GT_WINDOW_FULLSCREEN
	videoMode |= SDL_FULLSCREEN;
#endif

#ifdef GT_USE_OPENGL_ES
	SDL_Surface* screen = SDL_SetVideoMode(w, h, 0, videoMode);
	surface = screen;

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	if (!SDL_GetWMInfo(&wmInfo)) {
		LogError("Error in SDL_GetWMInfo()");
		exit(-1);
	}

	eglDisplay = eglGetDisplay((EGLNativeDisplayType) wmInfo.info.x11.display);

	if (eglDisplay == EGL_NO_DISPLAY) {
		LogError("No EGL display available!");
		return;
	}

	EGLint major, minor;

	if (!eglInitialize(eglDisplay, &major, &minor)) {
		LogError("Could not initialize EGL (error code 0x%X)!", eglGetError());
		return;
	}

	EGLint eglAttribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_RED_SIZE, 5,
			EGL_GREEN_SIZE, 5,
			EGL_BLUE_SIZE, 5,
			EGL_ALPHA_SIZE, 0,
			EGL_DEPTH_SIZE, 24,
			EGL_NONE
	};


	EGLint numConfigs;

    if (!eglGetConfigs(eglDisplay, NULL, 0, &numConfigs)) {
        LogError("Unable to get EGL configs (error code 0x%X)!", eglGetError());
        return;
    }


    LogInfo("************* EGL CONFIGS *************");

    LogInfo("#  R  G  B  A  D  S  CAV  CONF ");
    LogInfo("-------------------------------");

    EGLConfig* configs = new EGLConfig[numConfigs];

    eglGetConfigs(eglDisplay, configs, numConfigs, &numConfigs);

    for (EGLint i = 0 ; i < numConfigs ; i++) {
        EGLint r, g, b, a, d, s, cav, conf;

        eglGetConfigAttrib(eglDisplay, configs[i], EGL_RED_SIZE, &r);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_GREEN_SIZE, &g);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_BLUE_SIZE, &b);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_ALPHA_SIZE, &a);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_DEPTH_SIZE, &d);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_STENCIL_SIZE, &s);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_CONFIG_CAVEAT, &cav);
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_CONFORMANT, &conf);

        char cfgLine[256];

        sprintf(cfgLine, "%-2d %-2d %-2d %-2d %-2d %-2d %-2d ", i, r, g, b, a, d, s);

        if (cav == EGL_NONE)
        	strcat(cfgLine, "none ");
        else if (cav == EGL_SLOW_CONFIG)
        	strcat(cfgLine, "slow ");
        else if (cav == EGL_NON_CONFORMANT_CONFIG)
        	strcat(cfgLine, "ncon ");

        if ((conf & EGL_OPENGL_ES_BIT) != 0)
        	strcat(cfgLine, "1");
        else
        	strcat(cfgLine, " ");

        if ((conf & EGL_OPENGL_ES2_BIT) != 0)
        	strcat(cfgLine, "2");
        else
        	strcat(cfgLine, " ");

        if ((conf & EGL_OPENVG_BIT) != 0)
        	strcat(cfgLine, "V");
        else
        	strcat(cfgLine, " ");

        if ((conf & EGL_OPENGL_BIT) != 0)
        	strcat(cfgLine, "G");
        else
        	strcat(cfgLine, " ");

        LogInfo("%s", cfgLine);
    }

    LogInfo("************* END EGL CONFIGS *************");


    EGLConfig config;

	if (!eglChooseConfig(eglDisplay, eglAttribs, &config, 1, &numConfigs)) {
		LogError("Could not choose config (error code 0x%X)!", eglGetError());
		return;
	}

	if (numConfigs < 1) {
		LogError("No matching config could be found (error code 0x%X)!", eglGetError());
		return;
	}

	EGLint redSize, greenSize, blueSize, alphaSize, depthSize;

	eglGetConfigAttrib(eglDisplay, config, EGL_RED_SIZE, &redSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_GREEN_SIZE, &greenSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_BLUE_SIZE, &blueSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_ALPHA_SIZE, &alphaSize);
	eglGetConfigAttrib(eglDisplay, config, EGL_DEPTH_SIZE, &depthSize);

	LogInfo("Framebuffer: R%d G%d B%d A%d D%d", redSize, greenSize, blueSize, alphaSize, depthSize);

	eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType) wmInfo.info.x11.window, NULL);

	if (eglSurface == EGL_NO_SURFACE) {
		LogError("Creating surface failed (error code 0x%X)!", eglGetError());
		return;
	}

	if (firstInitialization) {
		EGLint eglContextAttribs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 2,
				EGL_NONE
		};

		eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, eglContextAttribs);

		if (eglContext == EGL_NO_CONTEXT) {
			LogError("Creating GLES2 context failed (error code 0x%X)!", eglGetError());
			return;
		}
	}

	if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
		LogError("Error making GLES2 context current (error code 0x%X)!", eglGetError());
		return;
	}

	firstInitialization = false;
#else
	const SDL_VideoInfo* info = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	surface = SDL_SetVideoMode(w, h, info->vfmt->BitsPerPixel, videoMode | SDL_OPENGL);

	int dsize;
	int r, g, b, a;
	int ssize;
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &dsize);
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a);
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &ssize);

	LogInfo("Framebuffer: R%d G%d B%d A%d D%d S%d", r, g, b, a, dsize, ssize);
#endif
}


int main(int argc, char** argv)
{
	try {
		SetLogLevel(LOG_LEVEL_DEBUG);

		LogInfo("Using gtatools version %s", GTATOOLS_VERSION);
		LogInfo("Using nxcommon version %s", NXCOMMON_VERSION);

		Controller controller;

		XMLDocument* configDoc = controller.getConfigDocument();

		File executableDir;

		try {
			executableDir = File::getExecutableFile().getParent();
		} catch (FileException& ex) {
			LogWarning("Executable directory could not be found. Cause: %s", ex.what());
		}

		File configFile("config.xml");

		if (!configFile.isRegularFile()) {
			configFile = File(executableDir, "config.xml");

			if (!configFile.isRegularFile()) {
				LogError("config.xml was not found!\n");
				exit(1);
			}
		}

		LogInfo("Using config.xml: %s", configFile.getCanonicalFile().toString().get());

		if (configDoc->LoadFile(configFile.toString().get()) != XML_NO_ERROR) {
			LogError("Error loading config.xml: %s\n", configDoc->GetErrorStr1());
			exit(1);
		}

		XMLElement* windowGeomElem = configDoc->RootElement()->FirstChildElement("window")->FirstChildElement("geometry");

		unsigned int windowWidth = windowGeomElem->UnsignedAttribute("width");
		unsigned int windowHeight = windowGeomElem->UnsignedAttribute("height");

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			LogError("Error Initializing SDL");
			exit(1);
		}

		// To prevent SDL from writing stdout/stderr to a file on Windows
		//freopen("CON", "w", stdout);
		//freopen("CON", "w", stderr);

		SDL_EnableUNICODE(1);
		//SDL_ShowCursor(SDL_DISABLE); // We'll use the CEGUI cursor

		initWindowSystem(windowWidth, windowHeight);

		SDL_WM_SetCaption(WINDOW_BASE_TITLE, NULL);

		controller.init();
		controller.reshape(windowWidth, windowHeight);

		uint64_t lastClickTime = 0;

		bool running = true;
		while (running) {
			if (!controller.paint()) {
				break;
			}

			glFlush();

	#ifdef GT_USE_OPENGL_ES
			eglSwapBuffers(eglDisplay, eglSurface);
	#else
			SDL_GL_SwapBuffers();
	#endif

			SDL_PumpEvents();

			SDL_Event lastMMEvt;
			bool hasMMEvt = false;

			SDL_Event evt;
			while (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_ALLEVENTS) > 0) {
				uint64_t time;

				if (evt.type != SDL_MOUSEBUTTONDOWN  &&  evt.type != SDL_MOUSEBUTTONUP) {
					lastClickTime = 0;
				}

				switch (evt.type) {
				case SDL_KEYDOWN:
					if (evt.key.keysym.sym == SDLK_q  &&  (evt.key.keysym.mod & KMOD_CTRL) != 0) {
						running = false;
						break;
					}

					controller.keyPressed(evt.key.keysym);
					break;
				case SDL_KEYUP:
					controller.keyReleased(evt.key.keysym);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (evt.button.button == SDL_BUTTON_LEFT) {
						time = GetTickcount();

						if (time-lastClickTime < 300) {
							controller.mouseButtonDoubleClicked(evt.button.x, evt.button.y);
							lastClickTime = 0;
						} else {
							lastClickTime = time;
						}
					} else {
						lastClickTime = 0;
					}

					controller.mouseButtonPressed(evt.button.button, evt.button.x, evt.button.y);
					break;
				case SDL_MOUSEBUTTONUP:
					controller.mouseButtonReleased(evt.button.button, evt.button.x, evt.button.y);
					break;
				case SDL_MOUSEMOTION:
					lastMMEvt = evt;
					hasMMEvt = true;
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
					controller.reshape(evt.resize.w, evt.resize.h);
					break;
				}
			}

			if (hasMMEvt) {
				controller.mouseMotion(lastMMEvt.motion.x, lastMMEvt.motion.y);
			}
		}

		controller.shutdown();

		LogInfo("Program terminated normally.");

		SDL_Quit();
	} catch (Exception& ex) {
		LogMultiError("Exception caught: %s", ex.what());
		CString bt = ex.getBacktrace();

		if (!bt.isNull()) {
			LogMultiError("STACK TRACE:\n----------------------------\n%s\n----------------------------", bt.get());
		}

		exit(1);
	}
}

