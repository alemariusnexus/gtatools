/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef PROFILEINITIALIZER_H_
#define PROFILEINITIALIZER_H_

#include <qthread.h>
#include <gtaformats/util/File.h>

class Profile;


class ProfileInitializer : public QThread {
	Q_OBJECT

public:
	ProfileInitializer(Profile* profile);
	void interrupt() { interrupted = true; }
	bool isInterrupted() { return interrupted; }

protected:
	virtual void run();

private:
	void addResource(const File& file);

private:
	Profile* profile;
	bool interrupted;
};

#endif /* PROFILEINITIALIZER_H_ */
