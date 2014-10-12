/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "COLEntityItemModel.h"




COLEntityItemModel::COLEntityItemModel(COLModel* model)
		: model(model)
{
	spheresEnabled = true;
	boxesEnabled = true;
	faceGroupsEnabled = true;
	facesEnabled = true;

	for (uint32_t i = 0 ; i < model->getSphereCount() ; i++) {
		enabledSpheres.insert(i);
	}
	for (uint32_t i = 0 ; i < model->getBoxCount() ; i++) {
		enabledBoxes.insert(i);
	}
	for (uint32_t i = 0 ; i < model->getFaceGroupCount() ; i++) {
		enabledFaceGroups.insert(i);
	}
	for (uint32_t i = 0 ; i < model->getFaceCount() ; i++) {
		enabledFaces.insert(i);
	}
}


Qt::ItemFlags COLEntityItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}


QVariant COLEntityItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	void* iptr = index.internalPointer();

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;
	const COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
	const COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;
	const COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;

	if (iptr == &sphereCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Spheres");
		} else if (role == Qt::CheckStateRole) {
			return spheresEnabled ? Qt::Checked : Qt::Unchecked;
		}
	} else if (iptr == &boxCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Boxes");
		} else if (role == Qt::CheckStateRole) {
			return boxesEnabled ? Qt::Checked : Qt::Unchecked;
		}
	} else if (iptr == &faceGroupCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Face Groups");
		} else if (role == Qt::CheckStateRole) {
			return faceGroupsEnabled ? Qt::Checked : Qt::Unchecked;
		}
	} else if (iptr == &faceCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Faces");
		} else if (role == Qt::CheckStateRole) {
			return facesEnabled ? Qt::Checked : Qt::Unchecked;
		}
	} else if (fgs  &&  iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Face Group %1").arg(((const COLFaceGroup*) iptr) - fgs);
		} else if (role == Qt::CheckStateRole) {
			return enabledFaceGroups.contains(((const COLFaceGroup*) iptr) - fgs) ? Qt::Checked : Qt::Unchecked;
		}
	} else if (spheres  &&  iptr >= spheres  &&  iptr <= spheres + (model->getSphereCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Sphere %1").arg(((const COLSphere*) iptr) - spheres);
		} else if (role == Qt::CheckStateRole) {
			return enabledSpheres.contains(((const COLSphere*) iptr) - spheres) ? Qt::Checked : Qt::Unchecked;
		}
	} else if (boxes  &&  iptr >= boxes  &&  iptr <= boxes + (model->getBoxCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Box %1").arg(((const COLBox*) iptr) - boxes);
		} else if (role == Qt::CheckStateRole) {
			return enabledBoxes.contains(((const COLBox*) iptr) - boxes) ? Qt::Checked : Qt::Unchecked;
		}
	} else if (faces  &&  iptr >= faces  &&  iptr <= faces + (model->getFaceCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Face %1").arg(((const COLFace*) iptr) - faces);
		} else if (role == Qt::CheckStateRole) {
			return enabledFaces.contains(((const COLFace*) iptr) - faces) ? Qt::Checked : Qt::Unchecked;
		}
	}

	return QVariant();
}


QVariant COLEntityItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}


int COLEntityItemModel::rowCount(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 3; // Spheres, Boxes, Faces/Face Groups
	}

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;

	void* iptr = index.internalPointer();

	if (iptr == &sphereCountDummy) {
		return model->getSphereCount();
	} else if (iptr == &boxCountDummy) {
		return model->getBoxCount();
	} else if (iptr == &faceGroupCountDummy) {
		return model->getFaceGroupCount();
	} else if (iptr == &faceCountDummy) {
		return model->getFaceCount();
	} else if (fgs  &&  iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		int fc = ((const COLFaceGroup*) iptr)->getFaceCount();
		return ((const COLFaceGroup*) iptr)->getFaceCount();
	}

	return 0;
}


QModelIndex COLEntityItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;

	if (parent.isValid()) {
		void * pptr = parent.internalPointer();

		if (pptr == &sphereCountDummy) {
			return createIndex(row, column, &model->getSpheres()[0] + row);
		} else if (pptr == &boxCountDummy) {
			return createIndex(row, column, &model->getBoxes()[0] + row);
		} else if (pptr == &faceGroupCountDummy) {
			return createIndex(row, column, &model->getFaceGroups()[0] + row);
		} else if (pptr == &faceCountDummy) {
			return createIndex(row, column, &model->getFaces()[0] + row);
		} else if (fgs  &&  pptr >= fgs  &&  pptr <= fgs + (model->getFaceGroupCount()-1)) {
			COLFaceGroup fg = *((const COLFaceGroup*) pptr);
			return createIndex(row, column, &model->getFaces()[0] + fg.getStartFace() + row);
		}
	} else {
		if (row == 0) {
			return createIndex(row, column, (void*) &sphereCountDummy);
		} else if (row == 1) {
			return createIndex(row, column, (void*) &boxCountDummy);
		} else {
			if (model->getFaceCount() != 0) {
				return createIndex(row, column, (void*) &faceGroupCountDummy);
			} else {
				return createIndex(row, column, (void*) &faceCountDummy);
			}
		}
	}

	return QModelIndex();
}


QModelIndex COLEntityItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;
	const COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
	const COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;
	const COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;

	void* iptr = index.internalPointer();

	if (spheres  &&  iptr >= spheres  &&  iptr <= spheres + (model->getSphereCount()-1)) {
		return this->index(0, 0);
	} else if (boxes  &&  iptr >= boxes  &&  iptr <= boxes + (model->getBoxCount()-1)) {
		return this->index(1, 0);
	} else if (fgs  &&  iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		return this->index(2, 0);
	} else if (faces  &&  iptr >= faces  &&  iptr <= faces + (model->getFaceCount()-1)) {
		if (model->getFaceGroupCount() != 0) {
			uint32_t faceIdx = ((const COLFace*) iptr) - faces;

			for (uint32_t i = 0 ; i < model->getFaceGroupCount() ; i++) {
				COLFaceGroup fg = fgs[i];

				if (faceIdx >= fg.getStartFace()  &&  faceIdx <= fg.getEndFace()) {
					return createIndex(i, 0, (void*) (fgs + i));
				}
			}
		} else {
			return this->index(2, 0);
		}
	}

	return QModelIndex();
}


bool COLEntityItemModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
	if (role == Qt::CheckStateRole) {
		bool checked = data.toInt() == Qt::Checked;

		uint32_t midx;
		ItemIndexType itype = getItemIndexType(index, midx);

		COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;
		COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
		COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;
		COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;

		if (itype == Spheres) {
			spheresEnabled = checked;

			for (uint32_t i : enabledSpheres) {
				emit sphereDisplayStateChanged(spheres + i, checked);
			}
		} else if (itype == Boxes) {
			boxesEnabled = checked;

			for (uint32_t i : enabledBoxes) {
				emit boxDisplayStateChanged(boxes + i, checked);
			}
		} else if (itype == FaceGroups) {
			faceGroupsEnabled = checked;

			for (uint32_t i : enabledFaceGroups) {
				COLFaceGroup* fg = fgs + i;

				for (uint32_t j = fg->getStartFace() ; j != fg->getEndFace()+1 ; j++) {
					if (enabledFaces.contains(j)) {
						emit faceDisplayStateChanged(faces + j, checked);
					}
				}
			}
		} else if (itype == Faces) {
			facesEnabled = checked;

			for (uint32_t i : enabledFaces) {
				emit faceDisplayStateChanged(faces + i, checked);
			}
		} else if (itype == Sphere) {
			if (checked) {
				enabledSpheres.insert(midx);
			} else {
				enabledSpheres.remove(midx);
			}

			if (spheresEnabled) {
				emit sphereDisplayStateChanged(spheres + midx, checked);
			}
		} else if (itype == Box) {
			if (checked) {
				enabledBoxes.insert(midx);
			} else {
				enabledBoxes.remove(midx);
			}

			if (boxesEnabled) {
				emit boxDisplayStateChanged(boxes + midx, checked);
			}
		} else if (itype == FaceGroup) {
			if (checked) {
				enabledFaceGroups.insert(midx);
			} else {
				enabledFaceGroups.remove(midx);
			}

			COLFaceGroup* fg = fgs + midx;

			if (faceGroupsEnabled) {
				for (uint32_t i = fg->getStartFace() ; i <= fg->getEndFace() ; i++) {
					if (enabledFaces.contains(i)) {
						emit faceDisplayStateChanged(faces + i, checked);
					}
				}
			}
		} else if (itype == Face) {
			if (checked) {
				enabledFaces.insert(midx);
			} else {
				enabledFaces.remove(midx);
			}

			bool actuallyToggled = false;

			if (!fgs) {
				actuallyToggled = facesEnabled;
			} else {
				actuallyToggled = faceGroupsEnabled;

				if (faceGroupsEnabled) {
					for (uint32_t i : enabledFaceGroups) {
						COLFaceGroup* fg = fgs + i;

						if (midx >= fg->getStartFace()  &&  midx <= fg->getEndFace()) {
							actuallyToggled = true;
							break;
						}
					}
				}
			}

			if (actuallyToggled) {
				emit faceDisplayStateChanged(faces + midx, checked);
			}
		}

		return true;
	}

	return false;
}


COLEntityItemModel::ItemIndexType COLEntityItemModel::getItemIndexType(const QModelIndex& idx, uint32_t& idxInModel)
{
	if (!idx.isValid()) {
		return Invalid;
	}

	void* iptr = idx.internalPointer();

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;
	const COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
	const COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;
	const COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;

	if (iptr == &sphereCountDummy) {
		return Spheres;
	} else if (iptr == &boxCountDummy) {
		return Boxes;
	} else if (iptr == &faceGroupCountDummy) {
		return FaceGroups;
	} else if (iptr == &faceCountDummy) {
		return Faces;
	} else if (fgs  &&  iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		idxInModel = ((const COLFaceGroup*) iptr) - fgs;
		return FaceGroup;
	} else if (spheres  &&  iptr >= spheres  &&  iptr <= spheres + (model->getSphereCount()-1)) {
		idxInModel = ((const COLSphere*) iptr) - spheres;
		return Sphere;
	} else if (boxes  &&  iptr >= boxes  &&  iptr <= boxes + (model->getBoxCount()-1)) {
		idxInModel = ((const COLBox*) iptr) - boxes;
		return Box;
	} else if (faces  &&  iptr >= faces  &&  iptr <= faces + (model->getFaceCount()-1)) {
		idxInModel = ((const COLFace*) iptr) - faces;
		return Face;
	}

	return Invalid;
}


