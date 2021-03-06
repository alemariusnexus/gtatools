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

#ifndef SYSTEMQUERYRESULT_H_
#define SYSTEMQUERYRESULT_H_

#include <QVariant>
#include <QHash>


class SystemQueryResult {
	typedef QHash<QString, QVariant> PropMap;

public:
	SystemQueryResult() {}
	void setProperty(const QString& name, const QVariant& value) { properties[name] = value; }
	const QVariant getProperty(const QString& name) { return properties[name]; }
	QVariant& operator[](const QString& name) { return properties[name]; }
	const QVariant operator[](const QString& name) const { return properties[name]; }

private:
	PropMap properties;
};

#endif /* SYSTEMQUERYRESULT_H_ */
