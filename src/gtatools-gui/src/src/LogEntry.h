/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
