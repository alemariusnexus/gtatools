/*
 * FirstStartWizard.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef FIRSTSTARTWIZARD_H_
#define FIRSTSTARTWIZARD_H_

#include <ui_FirstStartWizard.h>
#include <qwizard.h>



class FirstStartWizard : public QWizard {
	Q_OBJECT

public:
	FirstStartWizard();

private slots:
	void finished(int result);

private:
	Ui_FirstStartWizard ui;
};

#endif /* FIRSTSTARTWIZARD_H_ */
