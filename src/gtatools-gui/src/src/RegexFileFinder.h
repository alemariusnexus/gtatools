/*
 * RegexFileFinder.h
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#ifndef REGEXFILEFINDER_H_
#define REGEXFILEFINDER_H_

#include <gtaformats/util/FileFinder.h>
#include <QtCore/QRegExp>



class RegexFileFinder : public FileFinder {
public:
	RegexFileFinder(const QRegExp& regex, bool exactMatch = true);
	const QRegExp& getRegex() const { return regex; }
	bool isExactMatch() const { return exactMatch; }
	void setRegex(const QRegExp& regex) { this->regex = regex; }
	void setExactMatch(bool em) { exactMatch = em; }
	virtual bool matches(const File& file);

private:
	QRegExp regex;
	bool exactMatch;
};

#endif /* REGEXFILEFINDER_H_ */
