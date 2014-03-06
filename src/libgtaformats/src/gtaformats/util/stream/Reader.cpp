#include "Reader.h"




CString Reader::readFixedLengthString(size_t len)
{
	char* buf = new char[len+1];
	buf[len] = '\0';
	size_t numRead = read(buf, len);
	return CString::from(buf, len+1);
}


CString Reader::readNullTerminatedString(bool* terminatorFound)
{
	CString str("");
	str.reserve(64);

	char buf[4096];

	bool terminated = false;
	size_t numRead;

	char* bufEnd = buf+sizeof(buf);

	do {
		char* dest;
		for (dest = buf ; dest != bufEnd  &&  !terminated  &&  numRead == 1 ; dest++) {
			numRead = read(dest, 1);

			if (*dest == '\0')
				terminated = true;
		}

		str.append(CString(buf, dest-buf));
	} while (!terminated  &&  numRead == 1);

	if (terminatorFound)
		*terminatorFound = terminated;

	return str;
}
