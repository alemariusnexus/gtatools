/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef RENDERINGENTITYGENERATOR_H_
#define RENDERINGENTITYGENERATOR_H_

#include "RenderingEntity.h"
#include "../scene/parts/VisualSceneObject.h"
#include "../scene/objects/MapSceneObject.h"
#include "../scene/objects/AnimatedMapSceneObject.h"
#include "../scene/objects/SimpleDynamicSceneObject.h"
#include "../scene/objects/Vehicle.h"
#include "RenderingMesh.h"
#include "RenderingSubmesh.h"
#include "TestShaderPlugin.h"
#include "ShaderPluginRegistry.h"
#include <gtaformats/util/math/Matrix4.h>
#include <list>
#include <map>
#include <set>

using std::list;
using std::map;
using std::set;



class RenderingEntityGenerator
{
public:
	RenderingEntityGenerator();
	void generate(list<VisualSceneObject*>::iterator beg, list<VisualSceneObject*>::iterator end, list<RenderingEntity*>& outList);

private:
	RenderingSubmesh* generateFromSubmesh(Submesh* submesh, TextureSource* texSrc, RenderingMesh* rmesh, bool debug = false);
	RenderingMesh* generateFromMesh(Mesh* mesh, MeshPointer* mptr, TextureSource* texSrc, const Matrix4& modelMat, bool transparent,
			MeshFrame* frame);

	void generateFromMapSceneObject(MapSceneObject* mobj, list<RenderingEntity*>& outList);
	void generateFromAnimatedMapSceneObject(AnimatedMapSceneObject* aobj, list<RenderingEntity*>& outList);
	void generateFromStaticMapSceneObjectLODInstance(MapSceneObject* mobj, MapSceneObjectLODInstance* lodInst, list<RenderingEntity*>& outList);
	void generateFromAnimatedMapSceneObjectLODInstance(AnimatedMapSceneObject* aobj, MapSceneObjectLODInstance* lodInst, list<RenderingEntity*>& outList);
	void generateFromSimpleDynamicSceneObject(SimpleDynamicSceneObject* obj, list<RenderingEntity*>& outList);
	void generateFromVehicle(Vehicle* veh, list<RenderingEntity*>& outList);

private:
	Matrix4 vMatrix;
	Matrix4 vpMatrix;
};

#endif /* RENDERINGENTITYGENERATOR_H_ */
