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

#ifndef DFFGEOMETRYPART_H_
#define DFFGEOMETRYPART_H_

#include <gtaformats/config.h>
#include "DFFMaterial.h"

class DFFGeometry;


/**	\brief Container for vertex indices into the data of a DFFGeometry.
 *
 * 	A DFFGeometryPart contains indices referring to the vertex data of it's assigned geometry, and the
 * 	material to be used for this part of the DFF mesh. How indices are interpreted depends on the face
 * 	format of the geometry (i.e. triangle lists or triangle strips).
 */
class DFFGeometryPart {
private:
	friend class DFFLoader;
	friend class DFFGeometry;

public:
	/**	\brief Creates a new DFFGeometryPart using the given index data.
	 *
	 * 	It is then not bound to any geometry, use DFFGeometry::addPart() to attach this part to a geometry.
	 *
	 * 	@param ic The number of indices.
	 * 	@param indices The index array.
	 */
	DFFGeometryPart(uint32_t ic, uint32_t* indices);

	/**	\brief Creates an empty DFFGeometryPart with 0 indices.
	 *
	 * 	It is then not bound to any geometry, use DFFGeometry::addPart() to attach this part to a geometry.
	 */
	DFFGeometryPart() : indexCount(0), indices(new uint32_t[0]), material(NULL), geometry(NULL) {}

	/**	\brief Copy constructor.
	 *
	 * 	@param other The part to copy. Note that <b>the new part is not automatically added to the geometry
	 * 		of the part to be copied!</b>
	 */
	DFFGeometryPart(const DFFGeometryPart& other);

	/**	\brief Destructor.
	 *
	 * 	Does <b>not</b> delete it's associated material, as that is part of the geometry, not of the part.
	 */
	~DFFGeometryPart();

	/**	\brief Returns the number of indices.
	 *
	 *	@return The number of indices.
	 */
	uint32_t getIndexCount() const { return indexCount; }

	/**	\brief Returns the index data.
	 *
	 * 	@return The indices.
	 */
	uint32_t* getIndices() const { return indices; }

	/**	\brief Returns the associated material.
	 *
	 * 	@return The associated material. Might be NULL if none is set.
	 */
	DFFMaterial* getMaterial() const { return material; }

	/**	\brief Assigns the indices of this part.
	 *
	 * 	This object takes the ownership of the index data it is called with. This method also deletes the
	 * 	previous index data this part had.
	 *
	 * 	@param count The number of indices.
	 * 	@param indices The indices.
	 */
	void setIndices(uint32_t count, uint32_t* indices)
			{ delete[] indices; this->indices = indices; indexCount = count; }

	/**	\brief Assigns a material.
	 *
	 * 	The material <b>must</b> be owned by this part's geometry. This also implies that there has to be a
	 * 	valid geometry assigned to this part to call this method.
	 *
	 * 	@param mat The material.
	 * 	@see DFFGeometry::addPart()
	 */
	void setMaterial(DFFMaterial* mat);

	/**	\brief Returns the associated geometry.
	 *
	 * 	@return The geometry, might be NULL if none was assigned yet.
	 */
	DFFGeometry* getGeometry() { return geometry; }

	/**	\brief Returns the associated geometry.
	 *
	 * 	@return The geometry, might be NULL if none was assigned yet.
	 */
	const DFFGeometry* getGeometry() const { return geometry; }

private:
	/**	\brief Internal.
	 *
	 * 	Used by the DFFGeometry::addPart() methods and such. Assigns the geometry if this part. This also
	 * 	throws an exception if this part currently has another geometry assigned.
	 *
	 *	@param The new geometry.
	 */
	void reparent(DFFGeometry* geom);

private:
	uint32_t indexCount;
	uint32_t* indices;
	DFFMaterial* material;
	DFFGeometry* geometry;
};

#endif /* DFFGEOMETRYPART_H_ */
