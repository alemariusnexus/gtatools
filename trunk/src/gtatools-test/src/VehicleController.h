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

#ifndef VEHICLECONTROLLER_H_
#define VEHICLECONTROLLER_H_

#include <SDL.h>
#include <gta/Engine.h>
#include <gta/scene/Scene.h>
#include <gta/scene/objects/SimpleDynamicSceneObject.h>
#include <gta/scene/objects/MapSceneObject.h>
#include <gta/scene/objects/Vehicle.h>
#include "SphericalCameraController.h"



class VehicleController
{
public:
	VehicleController();

	void init();
	uint64_t update(uint64_t timePassed);

	void keyPressed(SDL_keysym evt);
	void keyReleased(SDL_keysym evt);
	void mouseButtonPressed(Uint8 button, int x, int y);
	void mouseButtonDoubleClicked(int x, int y);
	void mouseButtonReleased(Uint8 button, int x, int y);
	void mouseMotion(int x, int y);

private:
	MapSceneObject* generateFloor();

private:
	Engine* engine;
	Scene* scene;
	SimpleDynamicSceneObject* chassis;
	bool forceActive;
	bool torqueActive;
	unsigned long numUpdates;
	SphericalCameraController* scc;
	Vehicle* veh;

	int lastMouseX, lastMouseY;
};

#endif /* VEHICLECONTROLLER_H_ */
