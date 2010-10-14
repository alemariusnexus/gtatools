#ifndef IDEGUIMODULE_H_
#define IDEGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include "../../Profile.h"
#include <QtGui/QAction>



class IDEGUIModule : public GUIModule {
	Q_OBJECT

public:
	IDEGUIModule();
	virtual ~IDEGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void onFindDefinition(bool checked);

private:
	QAction* findDefinitionAction;
};

#endif /* IDEGUIMODULE_H_ */
