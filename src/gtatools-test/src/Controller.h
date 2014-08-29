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

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <SDL.h>
#include <nxcommon/file/File.h>
#include <gta/scene/parts/VisualSceneObject.h>
#include <gta/render/TestShaderPlugin.h>
#include <gta/scene/objects/Vehicle.h>
#include "VehicleController.h"
#include "BulletGLDebugDraw.h"


class Controller {
public:
	Controller();
	void init();
	void reshape(int w, int h);
	bool paint();
	void shutdown();
	void keyPressed(SDL_keysym evt);
	void keyReleased(SDL_keysym evt);
	void mouseButtonPressed(Uint8 button, int x, int y);
	void mouseButtonDoubleClicked(int x, int y);
	void mouseButtonReleased(Uint8 button, int x, int y);
	void mouseMotion(int x, int y);

private:
	void addResource(const File& file);

private:
	uint64_t lastFrameStart, lastMeasuredFrameStart;
	float moveFactor;
	float moveForwardFactor, moveSidewardFactor, moveUpFactor;
	bool firstFrame;
	int framesSinceLastMeasure;
	int lastMouseX, lastMouseY;
	bool printCacheStatistics;
	BulletGLDebugDraw* debugDrawer;
	bool programRunning;
	bool forceStatisticsUpdate;
	bool freeRunning;
	bool increaseTime;
	bool increaseTimeHold;

	VehicleController* veh;
};

#endif /* CONTROLLER_H_ */
