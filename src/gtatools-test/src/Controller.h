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

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <SDL.h>
#include <gtaformats/util/File.h>
#include <CEGUI/CEGUI.h>


class Controller {
public:
	Controller();
	void init();
	void reshape(int w, int h);
	bool paint();
	void keyPressed(SDL_keysym key);
	void keyReleased(SDL_keysym key);
	void mouseButtonPressed(Uint8 button, Uint16 x, Uint16 y);
	void mouseButtonReleased(Uint8 button, Uint16 x, Uint16 y);
	void mouseMotion(Uint16 x, Uint16 y);

private:
	void addResource(const File& file);

private:
	uint64_t lastFrameStart, lastMeasuredFrameStart, lastCEGUITime;
	float moveFactor;
	float moveForwardFactor, moveSidewardFactor, moveUpFactor;
	bool firstFrame;
	int framesSinceLastMeasure;
	int lastMouseX, lastMouseY;
	bool printCacheStatistics;
	//BulletGLDebugDraw* debugDrawer;
	bool programRunning;
	bool forceStatisticsUpdate;
};

#endif /* CONTROLLER_H_ */
