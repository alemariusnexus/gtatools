/*
 * IFPAnimationViewerEntity.h
 *
 *  Created on: 27.05.2012
 *      Author: alemariusnexus
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
