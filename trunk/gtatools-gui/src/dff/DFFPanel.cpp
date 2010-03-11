#include "DFFPanel.h"
#include <dff/DFFLoader.h>
#include <dff/DFFFrame.h>
#include "../lang/lang.h"



DFFPanel::DFFPanel( wxWindow* parent )
		: DFFPanelPrototype( parent )
{
	notebook->SetPageText(0, LangGet("DFFPanel_framePanel_label"));
	notebook->SetPageText(1, LangGet("DFFPanel_geometriesPanel_label"));

	frameOffsetDescLabel->SetLabel(LangGet("DFFPanel_frameOffsetDescLabel_label"));
	frameRotationDescLabel->SetLabel(LangGet("DFFPanel_frameRotationDescLabel_label"));
	frameParentDescLabel->SetLabel(LangGet("DFFPanel_frameParentDescLabel_label"));

	geometryNotebook->SetPageText(0, LangGet("DFFPanel_geometryInfoPanel_label"));
	geometryNotebook->SetPageText(1, LangGet("DFFPanel_geometryMaterialsPanel_label"));
	geometryNotebook->SetPageText(2, LangGet("DFFPanel_geometryPartsPanel_label"));

	geometryFormatDescLabel->SetLabel(LangGet("DFFPanel_geometryFormatDescLabel_label"));
	geometryFlagsDescLabel->SetLabel(LangGet("DFFPanel_geometryFlagsDescLabel_label"));
	geometryUVCountDescLabel->SetLabel(LangGet("DFFPanel_geometryUVCountDescLabel_label"));
	geometryVerticesDescLabel->SetLabel(LangGet("DFFPanel_geometryVerticesDescLabel_label"));
	geometryBoundsDescLabel->SetLabel(LangGet("DFFPanel_geometryBoundsDescLabel_label"));
	geometryMaterialsDescLabel->SetLabel(LangGet("DFFPanel_geometryMaterialsDescLabel_label"));
	geometryPartsDescLabel->SetLabel(LangGet("DFFPanel_geometryPartsDescLabel_label"));
	geometryFrameDescLabel->SetLabel(LangGet("DFFPanel_geometryFrameDescLabel_label"));

	geometryMaterialColorDescLabel->SetLabel(LangGet("DFFPanel_geometryMaterialColorDescLabel_label"));
	geometryMaterialTexturesSizer->GetStaticBox()->SetLabel(
			LangGet("DFFPanel_geometryMaterialTexturesSizer_label"));
	geometryMaterialTextureDiffnameDescLabel->SetLabel(
			LangGet("DFFPanel_geometryMaterialTextureDiffnameDescLabel_label"));
	geometryMaterialTextureAlphaDescLabel->SetLabel(
			LangGet("DFFPanel_geometryMaterialTextureAlphaDescLabel_label"));
	geometryMaterialTextureFilterDescLabel->SetLabel(
			LangGet("DFFPanel_geometryMaterialTextureFilterDescLabel_label"));
	geometryPartIndicesDescLabel->SetLabel(LangGet("DFFPanel_geometryPartIndicesDescLabel_label"));
	geometryPartMaterialDescLabel->SetLabel(LangGet("DFFPanel_geometryPartMaterialDescLabel_label"));
}


bool DFFPanel::doDisplay(DataSource* source)
{
	DFFLoader loader;
	mesh = loader.loadMesh(source->getStream());

	DFFFrame** frames = mesh->getFrames();

	for (int32_t i = 0 ; i < mesh->getFrameCount() ; i++) {
		DFFFrame* frame = frames[i];
		frameList->Append(wxString(frame->getName(), wxConvUTF8), frame);
	}

	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		DFFGeometry* geom = geoms[i];
		geometryList->Append(wxString::Format(wxT("Geometry %d"), i+1), geom);
	}

	return true;
}


void DFFPanel::doClose()
{
	delete mesh;
}


void DFFPanel::onFrameSelectionChanged(wxCommandEvent& evt)
{
	int sel = frameList->GetSelection();

	if (sel != -1) {
		DFFFrame* frame = (DFFFrame*) frameList->GetClientData(sel);
		const float* offs = frame->getTranslation();
		const float* rot = frame->getRotation();

		frameNameLabel->SetLabel(wxString(frame->getName(), wxConvUTF8));
		frameOffsetLabel->SetLabel(wxString::Format(wxT("%g, %g, %g"), offs[0], offs[1], offs[2]));
		frameRotationLabel->SetLabel(wxString::Format(wxT("(%g %g %g) (%g %g %g) (%g %g %g)"),
				rot[0], rot[1], rot[2], rot[3], rot[4], rot[5], rot[6], rot[7], rot[8]));

		DFFFrame* parent = frame->getParent();
		if (parent) {
			frameParentLabel->SetLabel(wxString(parent->getName(), wxConvUTF8));
		} else {
			frameParentLabel->SetLabel(LangGet("DFFPanel_frameParentLabel_noParent_label"));
		}
	}
}


void DFFPanel::onGeometrySelectionChanged(wxCommandEvent& evt)
{
	geometryMaterialsList->Clear();
	geometryPartsList->Clear();

	int sel = geometryList->GetSelection();

	if (sel != -1) {
		DFFGeometry* geom = (DFFGeometry*) geometryList->GetClientData(sel);
		const DFFBoundingSphere* bounds = geom->getBounds();

		geometryFormatLabel->SetLabel(geom->isTriangleStripFormat()
				? wxT("Triangle strips") : wxT("Triangle list"));
		geometryFlagsLabel->SetLabel(wxString::Format(wxT("%d"), geom->getFlags()));
		geometryUVCountLabel->SetLabel(wxString::Format(wxT("%d"), geom->getUVSetCount()));
		geometryVerticesLabel->SetLabel(wxString::Format(wxT("%d"), geom->getVertexCount()));
		geometryBoundsLabel->SetLabel(wxString::Format(wxT("%g %g %g %g"),
				bounds->x, bounds->y, bounds->z, bounds->radius));
		geometryMaterialsLabel->SetLabel(wxString::Format(wxT("%d"), geom->getMaterialCount()));
		geometryPartsLabel->SetLabel(wxString::Format(wxT("%d"), geom->getPartCount()));
		geometryFrameLabel->SetLabel(wxString(geom->getAssociatedFrame()->getName(), wxConvUTF8));

		renderer->setGeometry(geom);
		renderer->Refresh();

		DFFMaterial** mats = geom->getMaterials();

		for (int32_t i = 0 ; i < geom->getMaterialCount() ; i++) {
			DFFMaterial* mat = mats[i];
			geometryMaterialsList->Append(wxString::Format(wxT("Material %d"), i+1), mat);
		}

		DFFGeometryPart** parts = geom->getParts();

		for (int32_t i = 0 ; i < geom->getPartCount() ; i++) {
			DFFGeometryPart* part = parts[i];
			geometryPartsList->Append(wxString::Format(wxT("Part %d"), i+1), part);
		}
	}
}


void DFFPanel::onGeometryPartSelectionChanged(wxCommandEvent& evt)
{
	int sel = geometryPartsList->GetSelection();

	if (sel != -1) {
		DFFGeometryPart* part = (DFFGeometryPart*) geometryPartsList->GetClientData(sel);

		int geomSel = geometryList->GetSelection();
		DFFGeometry* geom = (DFFGeometry*) geometryList->GetClientData(geomSel);

		geometryPartIndicesLabel->SetLabel(wxString::Format(wxT("%d"), part->getIndexCount()));
		geometryPartMaterialLabel->SetLabel(wxString::Format(wxT("Material %d"),
				geom->indexOf(part->getMaterial())+1));
	}
}


void DFFPanel::onGeometryMaterialSelectionChanged(wxCommandEvent& evt)
{
	int sel = geometryMaterialsList->GetSelection();

	if (sel != -1) {
		DFFMaterial* mat = (DFFMaterial*) geometryMaterialsList->GetClientData(sel);
		uint8_t r, g, b, a;
		mat->getColor(r, g, b, a);

		geometryMaterialColorLabel->SetLabel(wxString::Format(wxT("#%02X%02X%02X%02X"), r, g, b, a));

		geometryMaterialTextureList->Clear();

		for (int32_t i = 0 ; i < mat->getTextureCount() ; i++) {
			DFFTexture* tex = mat->getTextures()[i];
			geometryMaterialTextureList->Append(wxString(tex->getDiffuseName(), wxConvUTF8), tex);
		}
	}
}


void DFFPanel::onGeometryMaterialTextureSelectionChanged(wxCommandEvent& evt)
{
	int sel = geometryMaterialTextureList->GetSelection();

	if (sel != -1) {
		DFFTexture* tex = (DFFTexture*) geometryMaterialTextureList->GetClientData(sel);

		geometryMaterialTextureDiffnameLabel->SetLabel(wxString(tex->getDiffuseName(), wxConvUTF8));
		geometryMaterialTextureAlphaLabel->SetLabel(wxString(tex->getAlphaName(), wxConvUTF8));
		geometryMaterialTextureFilterLabel->SetLabel(wxString::Format(wxT("%d"),
				tex->getFilterModeFlags()));
	}
}

