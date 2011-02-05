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

#ifndef SYSTEMQUERY_H_
#define SYSTEMQUERY_H_

#include <QtCore/QString>
#include <QtCore/QHash>


class SystemQuery {
	typedef QHash<QString, QVariant> DataMap;

public:
	SystemQuery(QString name) : name(name) {}
	void setProperty(const QString& name, const QVariant& value) { data[name] = value; }
	const QVariant getProperty(const QString& name) const { return data[name]; }
	QVariant& operator[](const QString& name) { return data[name]; }
	const QVariant operator[](const QString& name) const { return data[name]; }
	QString getName() const { return name; }

private:
	QString name;
	DataMap data;

};

#endif /* SYSTEMQUERY_H_ */
