/*
 * ItemDefinition.h
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#ifndef ITEMDEFINITION_H_
#define ITEMDEFINITION_H_

#include "Engine.h"
#include "Mesh.h"
#include <gtaformats/ide/IDEStaticObject.h>



class ItemDefinition {
public:
	ItemDefinition(hash_t meshHash, hash_t txdHash, float drawDistance);
	ItemDefinition(const IDEStaticObject& object);
	hash_t getMeshHash() const { return meshHash; }
	hash_t getTXDHash() const { return txdHash; }
	float getDrawDistance() const { return drawDistance; }
	Mesh* getMesh();
	virtual void render();

private:
	hash_t meshHash;
	hash_t txdHash;
	float drawDistance;
};

#endif /* ITEMDEFINITION_H_ */
