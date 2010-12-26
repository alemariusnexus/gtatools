/*
 * ItemDefinition.h
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#ifndef ITEMDEFINITION_H_
#define ITEMDEFINITION_H_

#include "config.h"
#include "Engine.h"
#include "Mesh.h"
#include <gtaformats/ide/IDEStaticObject.h>
#include "MeshPointer.h"
#include "TextureSource.h"



class ItemDefinition {
public:
	ItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, float drawDistance);
	ItemDefinition(const IDEStaticObject& object);
	~ItemDefinition();
	float getDrawDistance() const { return drawDistance; }
	virtual void render();

private:
	MeshPointer* meshPtr;
	TextureSource* texSrc;
	//hash_t txdHash;
	float drawDistance;
};

#endif /* ITEMDEFINITION_H_ */
