/*
 * SystemQueryResult.h
 *
 *  Created on: 29.01.2011
 *      Author: alemariusnexus
 */

#ifndef SYSTEMQUERYRESULT_H_
#define SYSTEMQUERYRESULT_H_

#include <QtCore/QVariant>
#include <QtCore/QHash>


class SystemQueryResult {
	typedef QHash<QString, QVariant> PropMap;

public:
	SystemQueryResult(bool successful = true) : successful(successful) {}
	void setProperty(const QString& name, const QVariant& value) { properties[name] = value; }
	const QVariant getProperty(const QString& name) { return properties[name]; }
	QVariant& operator[](const QString& name) { return properties[name]; }
	const QVariant operator[](const QString& name) const { return properties[name]; }
	bool isSuccessful() const { return successful; }
	void setSuccessful(bool successful) { this->successful = successful; }

private:
	bool successful;
	PropMap properties;
};

#endif /* SYSTEMQUERYRESULT_H_ */
