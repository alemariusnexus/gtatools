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

#ifndef DFFGEOMETRY_H_
#define DFFGEOMETRY_H_

#include <gtaformats/config.h>
#include "DFFGeometryPart.h"
#include "DFFMaterial.h"
#include "DFFFrame.h"
#include "../util/math/Matrix4.h"
#include <vector>
#include <algorithm>

using std::vector;
using std::find;


#define GEOMETRY_FLAG_TRISTRIP (1<<0)
#define GEOMETRY_FLAG_POSITIONS (1<<1)
#define GEOMETRY_FLAG_TEXCOORDS (1<<2)
#define GEOMETRY_FLAG_COLORS (1<<3)
#define GEOMETRY_FLAG_NORMALS (1<<4)
#define GEOMETRY_FLAG_DYNAMICLIGHTING (1<<5)
#define GEOMETRY_FLAG_MODULATEMATCOLOR (1<<6)
#define GEOMETRY_FLAG_MULTIPLEUVSETS (1<<7)


class DFFMesh;

struct DFFBoundingSphere {
	float x;
	float y;
	float z;
	float radius;
};


/**	\brief A DFF mesh geometry.
 *
 *	DFF geometries contain all the vertex information (positions, normals, colors, texture coords), as well
 *	as some flags and (depending on the DFF version) some other properties.
 *
 *	Every DFF geometry has a number of parts and materials, as well as an associated frame which describes
 *	it's translation relative to the DFF mesh root.
 *
 *	@see DFFGeometryPart
 *	@see DFFMaterial
 *	@see DFFMesh
 */
class DFFGeometry {
public:
	typedef vector<DFFMaterial*>::iterator MaterialIterator;
	typedef vector<DFFMaterial*>::const_iterator ConstMaterialIterator;
	typedef vector<DFFGeometryPart*>::iterator PartIterator;
	typedef vector<DFFGeometryPart*>::const_iterator ConstPartIterator;

private:
	friend class DFFLoader;
	friend class DFFMesh;

public:
	/**	\brief Creates a new DFFGeometry using the given vertex data.
	 *
	 * 	The DFFGeometry will take ownership of the data passed as parameters.
	 *
	 * 	@param numVertices The number of vertices.
	 * 	@param vertices The vertex positions.
	 * 	@param normals The vertex normals.
	 * 	@param uvCoords The UV coordinate set(s).
	 * 	@param uvSetCount The number of independent texture coordinate sets.
	 * 	@param vertexColors The vertex colors.
	 */
	DFFGeometry(uint32_t numVertices, float* vertices, float* normals = NULL, float* uvCoords = NULL,
			uint8_t uvSetCount = 0, uint8_t* vertexColors = NULL, uint8_t* boneIndices = NULL,
			float* boneWeights = NULL);

	/**	\brief Copy constructor.
	 */
	DFFGeometry(const DFFGeometry& other);

	/**	\brief Destructor.
	 *
	 *	Destroys this DFFGeometry, including all it's parts and materials.
	 */
	~DFFGeometry();

	/**	\brief Determines whether this geometry's vertex data is is stored in triangle strip format.
	 *
	 * 	@return true if it's a triangle strip format, false in case of plain triangle lists.
	 */
	bool isTriangleStripFormat() const { return (flags & GEOMETRY_FLAG_TRISTRIP) != 0; }

	bool isDynamicLightingEnabled() const { return (flags & GEOMETRY_FLAG_DYNAMICLIGHTING) != 0; }

	/**	\brief Returns the geometry flags.
	 *
	 *	@return The geometry flags.
	 */
	uint16_t getFlags() const { return flags; }

	/**	\brief Returns the number of independent UV coord sets.
	 *
	 * 	@return The number of UV coord sets.
	 */
	uint8_t getUVSetCount() const { return uvSetCount; }

	/**	\brief Returns the number of vertices.
	 *
	 * 	@return The number of vertices.
	 */
	uint32_t getVertexCount() const { return vertexCount; }

	/**	\brief Returns the number of frames.
	 *
	 * 	@return The number of frames.
	 * 	@todo Actually, a geometry only stores one (root) frame, so what is this value for?
	 */
	uint32_t getFrameCount() const { return frameCount; }

	/**	\brief Returns the ambient light propery.
	 *
	 * 	If loaded through DFFLoader, this is <b>only valid for RW versions < 3.4</b>, otherwise it's 0.0f.
	 *
	 *	@return The ambient light property.
	 */
	float getAmbientLight() const { return ambientLight; }

	/**	\brief Returns the diffuse light propery.
	 *
	 * 	If loaded through DFFLoader, this is <b>only valid for RW versions < 3.4</b>, otherwise it's 0.0f.
	 *
	 *	@return The diffuse light property.
	 */
	float getDiffuseLight() const { return diffuseLight; }

	/**	\brief Returns the specular light propery.
	 *
	 * 	If loaded through DFFLoader, this is <b>only valid for RW versions < 3.4</b>, otherwise it's 0.0f.
	 *
	 *	@return The specular light property.
	 */
	float getSpecularLight() const { return specularLight; }

	/**	\brief Returns the vertex color array.
	 *
	 * 	It is in R8G8B8A8 format.
	 *
	 * 	@return The vertex colors or NULL if there are none.
	 */
	uint8_t* getVertexColors() { return vertexColors; }

	/**	\brief Returns the vertex color array.
	 *
	 * 	It is in R8G8B8A8 format.
	 *
	 * 	@return The vertex colors or NULL if there are none.
	 */
	const uint8_t* getVertexColors() const { return vertexColors; }

	/**	\brief Returns the UV coordinate sets.
	 *
	 * 	It is a continuous array of texture coordinate sets.
	 *
	 * 	@return The UV coordinate sets.
	 */
	float* getUVCoordSets() { return uvCoordSets; }

	/**	\brief Returns the UV coordinate sets.
	 *
	 * 	It is a continuous array of texture coordinate sets.
	 *
	 * 	@return The UV coordinate sets.
	 */
	const float* getUVCoordSets() const { return uvCoordSets; }

	/**	\brief Returns a single UV coordinate set.
	 *
	 *	@param idx The UV coord set index.
	 *	@return The requested UV coord set.
	 */
	float* getUVCoordSet(uint8_t idx = 0);

	/**	\brief Returns a single UV coordinate set.
	 *
	 *	@param idx The UV coord set index.
	 *	@return The requested UV coord set.
	 */
	const float* getUVCoordSet(uint8_t idx = 0) const;

	/**	\brief Returns the bounding sphere of this geometry.
	 *
	 * 	@return The bounding sphere.
	 */
	const DFFBoundingSphere* getBounds() const { return bounds; }

	/**	\brief Returns the bounding sphere of this geometry.
	 *
	 * 	@return The bounding sphere.
	 */
	DFFBoundingSphere* getBounds() { return bounds; }

	/**	\brief Returns the vertex positions.
	 *
	 * 	@return The array of vertex positions.
	 */
	float* getVertices() { return vertices; }

	/**	\brief Returns the vertex positions.
	 *
	 * 	@return The array of vertex positions.
	 */
	const float* getVertices() const { return vertices; }

	/**	\brief Returns the vertex normals.
	 *
	 * 	@return The array of vertex normals.
	 */
	float* getNormals() { return normals; }

	/**	\brief Returns the vertex normals.
	 *
	 * 	@return The array of vertex normals.
	 */
	const float* getNormals() const { return normals; }

	uint8_t* getBoneIndices() { return boneIndices; }
	const uint8_t* getBoneIndices() const { return boneIndices; }

	float* getBoneWeights() { return boneWeights; }
	const float* getBoneWeights() const { return boneWeights; }

	/**	\brief Returns the number of materials.
	 *
	 * 	@return The number of materials.
	 */
	uint32_t getMaterialCount() const { return materials.size(); }

	/**	\brief Returns the number of parts.
	 *
	 * 	 @return The number of parts.
	 */
	uint32_t getPartCount() const { return parts.size(); }

	/**	\brief Returns the index of the given material.
	 *
	 * 	@param mat The material.
	 * 	@return The index or -1 if the material is not registered in this geometry.
	 */
	int32_t indexOf(DFFMaterial* mat) const;

	int32_t indexOf(DFFGeometryPart* part) const
	{
		ConstPartIterator it = find(parts.begin(), parts.end(), part);
		return it == parts.end() ? -1 : it-parts.begin();
	}

	/**	\brief Returns the frame associated with this geometry.
	 *
	 * 	@return The associated frame.
	 */
	DFFFrame* getAssociatedFrame() { return associatedFrame; }

	/**	\brief Returns the frame associated with this geometry.
	 *
	 * 	@return The associated frame.
	 */
	const DFFFrame* getAssociatedFrame() const { return associatedFrame; }

	/**	\brief Assigns the geometry flags.
	 *
	 * 	@param flags The geometry flags.
	 */
	void setFlags(uint16_t flags) { this->flags = flags; }

	/**	\brief Assigns the number of frames.
	 *
	 * 	@param fc The number of frames.
	 * 	@see getFrameCount()
	 */
	void setFrameCount(uint32_t fc) { frameCount = fc; }

	/**	\brief Assigns the amount of ambient light.
	 *
	 * 	Note that this property is used only in DFF files for <b>RW versions < 3.4</b>!
	 *
	 * 	@param al The amount of ambient light.
	 */
	void setAmbientLight(float al) { ambientLight = al; }

	/**	\brief Assigns the amount of diffuse light.
	 *
	 * 	Note that this property is used only in DFF files for <b>RW versions < 3.4</b>!
	 *
	 * 	@param dl The amount of diffuse light.
	 */
	void setDiffuseLight(float dl) { diffuseLight = dl; }

	/**	\brief Assigns the amount of specular light.
	 *
	 * 	Note that this property is used only in DFF files for <b>RW versions < 3.4</b>!
	 *
	 * 	@param sl The amount of specular light.
	 */
	void setSpecularLight(float sl) { specularLight = sl; }

	/**	\brief Sets the bounding sphere of this geometry.
	 *
	 * 	This geometry will take ownership of the object passed.
	 *
	 *	@param bounds The bounding sphere.
	 */
	void setBounds(DFFBoundingSphere* bounds) { this->bounds = bounds; }

	/**	\brief Assigns the vertex data.
	 *
	 * 	The geometry takes ownership of all the arrays passed in as parameters.
	 *
	 * 	@param numVertices The number of vertices.
	 * 	@param vertices The vertex array.
	 * 	@param normals The normals array.
	 * 	@param uvCoords The UV coordinate set(s).
	 * 	@param uvSetCount The number of independent UV coordinate sets.
	 * 	@param vertexColors The vertex color array.
	 */
	void setVertices(uint32_t numVertices, float* vertices, float* normals = NULL, float* uvCoords = NULL,
			uint8_t uvSetCount = 0, uint8_t* vertexColors = NULL, uint8_t* boneIndices = NULL,
			float* boneWeights = NULL);

	/**	\brief Returns the material list begin iterator.
	 *
	 * 	@return The material iterator.
	 */
	MaterialIterator getMaterialBegin() { return materials.begin(); }

	/**	\brief Returns the material list end iterator.
	 *
	 * 	@return The material iterator.
	 */
	MaterialIterator getMaterialEnd() { return materials.end(); }

	/**	\brief Returns the material list begin iterator.
	 *
	 * 	@return The material iterator.
	 */
	ConstMaterialIterator getMaterialBegin() const { return materials.begin(); }

	/**	\brief Returns the material list end iterator.
	 *
	 * 	@return The material iterator.
	 */
	ConstMaterialIterator getMaterialEnd() const { return materials.end(); }

	/**	\brief Returns the material at the given index.
	 *
	 * 	@param index The material index.
	 * 	@return The material at the given index.
	 */
	DFFMaterial* getMaterial(uint32_t index);

	/**	\brief Returns the material at the given index.
	 *
	 * 	@param index The material index.
	 * 	@return The material at the given index.
	 */
	const DFFMaterial* getMaterial(uint32_t index) const;

	/**	\brief Add a material.
	 *
	 * 	The material must not be assigned to another geometry, otherwise an exception will be thrown.
	 *
	 * 	@param material The new material.
	 */
	void addMaterial(DFFMaterial* material) { materials.push_back(material); material->reparent(this); }

	/**	\brief Removes the material at the given index.
	 *
	 * 	The material will not be deleted.
	 *
	 * 	@param index The material index.
	 */
	void removeMaterial(uint32_t index) { removeMaterial(getMaterial(index)); }

	/**	\brief Removes the given material.
	 *
	 * 	It will not be deleted.
	 *
	 * 	@param material The material to remove.
	 */
	void removeMaterial(DFFMaterial* material);

	/**	\brief Removes all materials.
	 *
	 * 	None of them will be deleted.
	 */
	void removeMaterials();

	/**	\brief Returns the geometry part list begin iterator.
	 *
	 * 	@return The part iterator.
	 */
	PartIterator getPartBegin() { return parts.begin(); }

	/**	\brief Returns the geometry part list end iterator.
	 *
	 * 	@return The part iterator.
	 */
	PartIterator getPartEnd() { return parts.end(); }

	/**	\brief Returns the geometry part list begin iterator.
	 *
	 * 	@return The part iterator.
	 */
	ConstPartIterator getPartBegin() const { return parts.begin(); }

	/**	\brief Returns the geometry part list end iterator.
	 *
	 * 	@return The part iterator.
	 */
	ConstPartIterator getPartEnd() const { return parts.end(); }

	Matrix4** getInverseBoneMatrices() const { return inverseBoneMatrices; }

	uint8_t getBoneCount() const { return boneCount; }


	DFFGeometryPart* getPart(unsigned int index);
	const DFFGeometryPart* getPart(unsigned int index) const;
	void addPart(DFFGeometryPart* part) { parts.push_back(part); part->reparent(this); }
	void removePart(int index) { removePart(getPart(index)); }
	void removePart(DFFGeometryPart* part);
	void removeParts();
	void setAssociatedFrame(DFFFrame* frame) { associatedFrame = frame; }

	void mirrorYZ();
	void mirrorUVHorizontal();
	void scale(float x, float y, float z);

private:
	void reparent(DFFMesh* mesh);

private:
	uint16_t flags;
	uint8_t uvSetCount;
	uint32_t vertexCount;
	uint32_t frameCount;

	float ambientLight;
	float diffuseLight;
	float specularLight;
	uint8_t* vertexColors;
	float* uvCoordSets;
	DFFBoundingSphere* bounds;
	float* vertices;
	float* normals;
	uint8_t* boneIndices;
	float* boneWeights;
	uint8_t boneCount;
	Matrix4** inverseBoneMatrices;

	vector<DFFMaterial*> materials;
	vector<DFFGeometryPart*> parts;

	DFFFrame* associatedFrame;
	DFFMesh* mesh;
};

#endif /* DFFGEOMETRY_H_ */
