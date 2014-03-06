#ifndef RESOURCEDATLOADER_H_
#define RESOURCEDATLOADER_H_

#include "../util/File.h"
#include <istream>

using std::istream;



class ResourceDATLoader
{
public:
	enum EntryType
	{
		IDE,
		IPL,
		IMG,
		TXD
	};

	struct Entry
	{
		EntryType type;
		CString rawPath;
		File resolvedFile;
	};

public:
	ResourceDATLoader(const File& rootDir) : rootDir(rootDir) {}
	void setRootDirectory(const File& rootDir) { this->rootDir = rootDir; }
	File getRootDirectory() const { return rootDir; }
	bool loadEntry(istream* stream, Entry& entry);

private:
	File rootDir;
};

#endif /* RESOURCEDATLOADER_H_ */
