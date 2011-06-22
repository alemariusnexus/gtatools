/*
 * LogEntry.h
 *
 *  Created on: 21.06.2011
 *      Author: alemariusnexus
 */

#ifndef LOGENTRY_H_
#define LOGENTRY_H_

#include <gtaformats/util/Exception.h>
#include <QtCore/QString>



class LogEntry {
public:
	enum Type
	{
		Debug,
		Info,
		Warning,
		Error
	};

public:
	LogEntry(const QString& msg, Type type = Error, Exception* ex = NULL)
			: message(msg), ex(ex ? ex->copy() : NULL), type(type) {}
	~LogEntry() { if (ex) delete ex; }
	void setMessage(const QString& msg) { message = msg; }
	void setException(const Exception& ex) { this-> ex = ex.copy(); }
	void setType(Type type) { this->type = type; }
	QString getMessage() const { return message; }
	Exception* getException() { return ex; }
	const Exception* getException() const { return ex; }
	Type getType() const { return type; }
	bool isError() const { return type == Error; }
	bool isWarning() const { return type == Warning; }
	bool isDebug() const { return type == Debug; }
	bool isInfo() const { return type == Info; }

public:
	static LogEntry error(const QString& msg, Exception* ex = NULL) { return LogEntry(msg, Error, ex); }
	static LogEntry warning(const QString& msg, Exception* ex = NULL) { return LogEntry(msg, Warning, ex); }
	static LogEntry debug(const QString& msg, Exception* ex = NULL) { return LogEntry(msg, Debug, ex); }
	static LogEntry info(const QString& msg, Exception* ex = NULL) { return LogEntry(msg, Info, ex); }

private:
	QString message;
	Exception* ex;
	Type type;
};

#endif /* LOGENTRY_H_ */
