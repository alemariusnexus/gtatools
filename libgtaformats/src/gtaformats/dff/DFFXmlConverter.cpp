/*
 * DFFXmlConverter.cpp
 *
 *  Created on: 28.02.2010
 *      Author: alemariusnexus
 */

#include "DFFXmlConverter.h"
#include <fstream>

using std::ofstream;


TiXmlDocument* DFFXmlConverter::convert(DFFMesh* mesh)
{
	TiXmlDocument* doc = new TiXmlDocument();

	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc->LinkEndChild(decl);

	TiXmlElement* eMesh = new TiXmlElement("mesh");
	doc->LinkEndChild(eMesh);

	TiXmlElement* eFrames = new TiXmlElement("frames");
	eFrames->SetAttribute("count", mesh->getFrameCount());
	eMesh->LinkEndChild(eFrames);

	TiXmlElement* eGeoms = new TiXmlElement("geometries");
	eGeoms->SetAttribute("count", mesh->getGeometryCount());
	eMesh->LinkEndChild(eGeoms);

	DFFFrame** frames = mesh->getFrames();
	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getFrameCount() ; i++) {
		DFFFrame* frame = frames[i];
		const float* offs = frame->getTranslation();
		const float* rot = frame->getRotation();

		TiXmlElement* eFrame = new TiXmlElement("frame");
		eFrame->SetAttribute("name", frame->getName() != NULL ? frame->getName() : "[none]");
		eFrame->SetAttribute("flags", frame->getFlags());
		eFrame->SetAttribute("parent", mesh->indexOf(frame->getParent()));
		eFrames->LinkEndChild(eFrame);

		TiXmlElement* eOffs = new TiXmlElement("offset");
		eOffs->SetDoubleAttribute("x", offs[0]);
		eOffs->SetDoubleAttribute("y", offs[1]);
		eOffs->SetDoubleAttribute("z", offs[2]);
		eFrame->LinkEndChild(eOffs);

		TiXmlElement* eRot = new TiXmlElement("rotation");
		eRot->SetDoubleAttribute("m00", rot[0]);
		eRot->SetDoubleAttribute("m01", rot[1]);
		eRot->SetDoubleAttribute("m02", rot[2]);
		eRot->SetDoubleAttribute("m10", rot[3]);
		eRot->SetDoubleAttribute("m11", rot[4]);
		eRot->SetDoubleAttribute("m12", rot[5]);
		eRot->SetDoubleAttribute("m20", rot[6]);
		eRot->SetDoubleAttribute("m21", rot[7]);
		eRot->SetDoubleAttribute("m22", rot[8]);
		eFrame->LinkEndChild(eRot);
	}

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		DFFGeometry* geom = geoms[i];

		const DFFBoundingSphere* bounds = geom->getBounds();
		int32_t vertexCount = geom->getVertexCount();

		TiXmlElement* eGeom = new TiXmlElement("geometry");
		eGeom->SetAttribute("vertex_format", geom->isTriangleStripFormat()
				? "triangle_strips" : "triangle_list");
		eGeom->SetAttribute("flags", geom->getFlags());
		eGeom->SetAttribute("vertex_count", vertexCount);
		eGeoms->LinkEndChild(eGeom);

		TiXmlElement* eLight = new TiXmlElement("light");
		eLight->SetAttribute("ambient", geom->getAmbientLight());
		eLight->SetAttribute("diffuse", geom->getDiffuseLight());
		eLight->SetAttribute("specular", geom->getSpecularLight());
		eGeom->LinkEndChild(eLight);

		TiXmlElement* eBounds = new TiXmlElement("bounds");
		eBounds->SetDoubleAttribute("x", bounds->x);
		eBounds->SetDoubleAttribute("y", bounds->y);
		eBounds->SetDoubleAttribute("z", bounds->z);
		eBounds->SetDoubleAttribute("r", bounds->radius);
		eGeom->LinkEndChild(eBounds);

		TiXmlElement* eVertices = new TiXmlElement("vertices");
		eVertices->SetAttribute("count", vertexCount);
		eGeom->LinkEndChild(eVertices);

		float* verts = geom->getVertices();
		float* normals = geom->getNormals();
		uint8_t* colors = geom->getVertexColors();
		float** uvSets = geom->getUVCoordSets();

		for (int32_t j = 0 ; j < vertexCount ; j++) {
			TiXmlElement* eVertex = new TiXmlElement("vertex");
			eVertices->LinkEndChild(eVertex);

			if (geom->getVertices() != NULL) {
				TiXmlElement* ePosition = new TiXmlElement("position");
				ePosition->SetDoubleAttribute("x", verts[(j*3)]);
				ePosition->SetDoubleAttribute("y", verts[(j*3) + 1]);
				ePosition->SetDoubleAttribute("z", verts[(j*3) + 2]);
				eVertex->LinkEndChild(ePosition);
			}

			if (geom->getNormals() != NULL) {
				TiXmlElement* eNormal = new TiXmlElement("normal");
				eNormal->SetDoubleAttribute("x", normals[(j*3)]);
				eNormal->SetDoubleAttribute("x", normals[(j*3) + 1]);
				eNormal->SetDoubleAttribute("x", normals[(j*3) + 2]);
				eVertex->LinkEndChild(eNormal);
			}

			if (geom->getVertexColors() != NULL) {
				TiXmlElement* eColor = new TiXmlElement("color");
				eColor->SetAttribute("r", colors[(j*4)]);
				eColor->SetAttribute("g", colors[(j*4) + 1]);
				eColor->SetAttribute("b", colors[(j*4) + 2]);
				eColor->SetAttribute("a", colors[(j*4) + 3]);
				eVertex->LinkEndChild(eColor);
			}

			if (geom->getUVCoordSets() != NULL) {
				TiXmlElement* eUVCoords = new TiXmlElement("uv_coords");
				eVertex->LinkEndChild(eUVCoords);

				for (int32_t k = 0 ; k < geom->getUVSetCount() ; k++) {
					TiXmlElement* eUVCoord = new TiXmlElement("uv_coord");
					eUVCoord->SetDoubleAttribute("u", uvSets[k][j*2]);
					eUVCoord->SetDoubleAttribute("v", uvSets[k][(j*2) + 1]);
					eUVCoords->LinkEndChild(eUVCoord);
				}
			}
		}

		TiXmlElement* eMaterials = new TiXmlElement("materials");
		eMaterials->SetAttribute("count", geom->getMaterialCount());
		eGeom->LinkEndChild(eMaterials);

		DFFMaterial** mats = geom->getMaterials();

		for (int32_t j = 0 ; j < geom->getMaterialCount() ; j++) {
			DFFMaterial* mat = mats[j];
			uint8_t r, g, b, a;
			mat->getColor(r, g, b, a);

			TiXmlElement* eMaterial = new TiXmlElement("material");
			eMaterials->LinkEndChild(eMaterial);

			TiXmlElement* eColor = new TiXmlElement("color");
			eColor->SetAttribute("r", r);
			eColor->SetAttribute("g", g);
			eColor->SetAttribute("b", b);
			eColor->SetAttribute("a", a);
			eMaterial->LinkEndChild(eColor);

			TiXmlElement* eTextures = new TiXmlElement("textures");
			eTextures->SetAttribute("count", mat->getTextureCount());
			eMaterial->LinkEndChild(eTextures);

			DFFTexture** texes = mat->getTextures();

			for (int32_t k = 0 ; k < mat->getTextureCount() ; k++) {
				DFFTexture* tex = texes[k];

				TiXmlElement* eTexture = new TiXmlElement("texture");
				eTexture->SetAttribute("diffuse_name", tex->getDiffuseName());
				eTexture->SetAttribute("alpha_name", tex->getAlphaName());
				eTexture->SetAttribute("filter_flags", tex->getFilterModeFlags());
				eTextures->LinkEndChild(eTexture);
			}
		}

		TiXmlElement* eParts = new TiXmlElement("parts");
		eParts->SetAttribute("count", geom->getPartCount());
		eGeom->LinkEndChild(eParts);

		DFFGeometryPart** parts = geom->getParts();

		for (int32_t j = 0 ; j < geom->getPartCount() ; j++) {
			DFFGeometryPart* part = parts[j];

			TiXmlElement* ePart = new TiXmlElement("part");
			ePart->SetAttribute("material", geom->indexOf(part->getMaterial()));
			eParts->LinkEndChild(ePart);

			TiXmlElement* eIndices = new TiXmlElement("indices");
			eIndices->SetAttribute("count", part->getIndexCount());
			ePart->LinkEndChild(eIndices);

			int32_t* indices = part->getIndices();

			for (int32_t k = 0 ; k < part->getIndexCount() ; k++) {
				TiXmlElement* eIndex = new TiXmlElement("index");
				eIndex->SetAttribute("v", indices[k]);
				eIndices->LinkEndChild(eIndex);
			}
		}
	}

	return doc;
}


bool DFFXmlConverter::save(DFFMesh* mesh, const char* filename)
{
	TiXmlDocument* doc = convert(mesh);
	bool stat = doc->SaveFile(filename);
	delete doc;
	return stat;
}
