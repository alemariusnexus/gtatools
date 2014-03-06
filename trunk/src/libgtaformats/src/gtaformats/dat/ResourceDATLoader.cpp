#include "ResourceDATLoader.h"

using std::streamoff;



bool ResourceDATLoader::loadEntry(istream* stream, Entry& entry)
{
	char line[4096];

	while (!stream->eof()) {
		stream->getline(line, sizeof(line));
		streamoff numChars = stream->gcount();

		if (numChars >= 2  &&  line[numChars-2] == '\r') {
			line[numChars-2] = '\0';
		}


		if (line[0] == '\0') {
			continue;
		}

		if (strncmp(line, "IDE", 3) == 0) {
			CString rawPath = CString(line+4);
			rawPath.trim();

			FilePath* path = new FilePath(*rootDir.getPath(), rawPath, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File file(path, true);

			entry.type = IDE;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;
		} else if (strncmp(line, "IPL", 3) == 0) {
			CString rawPath = CString(line+4);
			rawPath.trim();

			FilePath* path = new FilePath(*rootDir.getPath(), rawPath, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File file(path, true);

			entry.type = IPL;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;

			/*FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File ipl(&path, false);
			iplRecurse(&ipl, rootDir, gameInfo);*/
		} else if (strncmp(line, "IMG", 3) == 0) {
			CString rawPath = CString(line+4);
			rawPath.trim();

			FilePath* path = new FilePath(*rootDir.getPath(), rawPath, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File file(path, true);

			entry.type = IMG;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;
		} else if (strncmp(line, "TEXDICTION", 10) == 0) {
			CString rawPath = CString(line+11);
			rawPath.trim();

			FilePath* path = new FilePath(*rootDir.getPath(), rawPath, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File file(path, true);

			entry.type = TXD;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;
		}
	}

	return false;
}
