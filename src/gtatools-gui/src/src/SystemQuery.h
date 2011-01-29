/*
 * SystemQuery.h
 *
 *  Created on: 29.01.2011
 *      Author: alemariusnexus
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
