/*
 * IDEFileFinder.h
 *
 *  Created on: 15.10.2010
 *      Author: alemariusnexus
 */

#ifndef IDEFILEFINDER_H_
#define IDEFILEFINDER_H_

#include <gtaformats/util/FileFinder.h>
#include <QtCore/QMap>



class IDEFileFinder : public FileFinder {
public:
	IDEFileFinder(int32_t id);
	int32_t getID() const { return id; }
	void setID(int32_t id) { this->id = id; }
	virtual bool matches(const File& file);
	int getMatchedLine(const File& file) const { return fileLines[file]; }

private:
	int32_t id;
	QMap<File, int> fileLines;
};

#endif /* IDEFILEFINDER_H_ */
