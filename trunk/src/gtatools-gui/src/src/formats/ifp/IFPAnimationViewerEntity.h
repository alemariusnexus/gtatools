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

#ifndef IFPANIMATIONVIEWERENTITY_H_
#define IFPANIMATIONVIEWERENTITY_H_

#include "../../DisplayedEntity.h"
#include "IFPAnimationViewerWidget.h"



class IFPAnimationViewerEntity : public DisplayedEntity
{
	Q_OBJECT

public:
	IFPAnimationViewerEntity(IFPAnimationViewerWidget* widget) : widget(widget) {}
	virtual ~IFPAnimationViewerEntity() { delete widget; }
	virtual QWidget* getWidget() const { return widget; }
	virtual QString getName() const { return tr("Animation Viewer"); }

private:
	IFPAnimationViewerWidget* widget;
};

#endif /* IFPANIMATIONVIEWERENTITY_H_ */
