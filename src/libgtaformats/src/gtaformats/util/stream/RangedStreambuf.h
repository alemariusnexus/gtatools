/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef RANGEDSTREAMBUF_H_
#define RANGEDSTREAMBUF_H_

#include <streambuf>
#include <ios>
#include <locale>
#include <algorithm>
#include "IOException.h"
#include <cstdio>

using std::basic_streambuf;
using std::char_traits;
using std::streampos;
using std::streamoff;
using std::streamsize;
using std::streamoff;
using std::ios_base;
using std::locale;
using std::min;


template <class charT, class traits = char_traits<charT> >
class RangedStreambuf : public basic_streambuf<charT, traits> {
public:
	typedef basic_streambuf<charT, traits> BackType;
	typedef typename BackType::char_type char_type;
	typedef typename BackType::int_type int_type;
	typedef typename BackType::off_type off_type;
	typedef typename BackType::pos_type pos_type;

public:
	RangedStreambuf(BackType& backend, streamsize maxWrite);

protected:
	virtual int_type overflow(int_type c);
	virtual streamsize xsputn(const char_type* s, streamsize n);
	virtual int sync();
	virtual pos_type seekoff(off_type off, ios_base::seekdir dir,
			ios_base::openmode mode = ios_base::in | ios_base::out);
	virtual pos_type seekpos(pos_type pos, ios_base::openmode mode = ios_base::in | ios_base::out);
	virtual void imbue(const locale& loc);
	virtual int_type pbackfail(int_type c = traits::eof());
	virtual streamsize showmanyc();
	virtual int_type uflow();
	virtual int_type underflow();
	virtual streamsize xsgetn(char_type* s, streamsize n);

private:
	BackType& backend;
	streamsize maxWrite;
	streamoff startPos;
	streamsize curReadPos;
	streamsize curWritePos;
};




template <class charT, class traits>
RangedStreambuf<charT, traits>::RangedStreambuf(basic_streambuf<charT, traits>& backend, streamsize maxWrite)
		: backend(backend), maxWrite(maxWrite), startPos(backend.pubseekoff(0, ios_base::cur))
{
	curReadPos = 0;
	curWritePos = 0;
}


template <class charT, class traits>
typename basic_streambuf<charT, traits>::int_type
RangedStreambuf<charT, traits>::overflow(typename basic_streambuf<charT, traits>::int_type c)
{
	if (curWritePos >= maxWrite) {
		return traits::eof();
	}

	curWritePos++;

	if (!traits::eq_int_type(c, traits::eof()))
		return backend.sputc(traits::to_char_type(c));
	else
		return traits::not_eof(c);
}


template <class charT, class traits>
streamsize RangedStreambuf<charT, traits>::xsputn(const char_type* s, streamsize n)
{
	n = min(n, maxWrite-curWritePos);
	n = backend.sputn(s, n);
	curWritePos += n;
	return n;
}


template <class charT, class traits>
int RangedStreambuf<charT, traits>::sync()
{
	return backend.pubsync();
}


template <class charT, class traits>
typename RangedStreambuf<charT, traits>::pos_type
RangedStreambuf<charT, traits>::seekoff(off_type off, ios_base::seekdir dir, ios_base::openmode mode)
{
	if (dir == ios_base::beg) {
		return seekpos(off, mode);
	} else if (dir == ios_base::cur) {
		if ((mode & ios_base::out) != 0) {
			pos_type absPos = off+curWritePos;
			if (absPos < 0  ||  absPos >= maxWrite) {
				throw IOException("Attempt to seek out of bounds!", __FILE__, __LINE__);
			}
		}
		if ((mode & ios_base::in) != 0) {
			pos_type absPos = off+curReadPos;
			if (absPos < 0  ||  absPos >= maxWrite) {
				throw IOException("Attempt to seek out of bounds!", __FILE__, __LINE__);
			}
		}
	} else {
		if (off > 0  ||  -off >= maxWrite) {
			throw IOException("Attempt to seek out of bounds!", __FILE__, __LINE__);
		}
	}

	if ((mode & ios_base::out) != 0) {
		pos_type oldWritePos = startPos+curWritePos;
		pos_type newWritePos = backend.pubseekoff(off, dir, ios_base::out);
		curWritePos += newWritePos-oldWritePos;
	}
	if ((mode & ios_base::in) != 0) {
		pos_type oldReadPos = startPos+curReadPos;
		pos_type newReadPos = backend.pubseekoff(off, dir, ios_base::in);
		curReadPos += newReadPos-oldReadPos;
		return curReadPos;
	}

	return curWritePos;
}


template <class charT, class traits>
typename RangedStreambuf<charT, traits>::pos_type
RangedStreambuf<charT, traits>::seekpos(pos_type pos, ios_base::openmode mode)
{
	if (pos >= maxWrite) {
		throw IOException("Attempt to seek out of bounds!", __FILE__, __LINE__);
	}

	if ((mode | ios_base::in) != 0) {
		curReadPos = pos;
	}
	if ((mode | ios_base::out) != 0) {
		curWritePos = pos;
	}

	return backend.pubseekpos(startPos+pos, mode);
}


template <class charT, class traits>
void RangedStreambuf<charT, traits>::imbue(const locale& loc)
{
	backend.pubimbue(loc);
}


template <class charT, class traits>
typename basic_streambuf<charT, traits>::int_type
RangedStreambuf<charT, traits>::pbackfail(int_type c)
{
	if (curReadPos <= startPos) {
		return traits::eof();
	}

	int_type retval = backend.sputbackc(c);

	if (!traits::eq_int_type(c, traits::eof())) {
		curReadPos--;
	}

	return retval;
}


template <class charT, class traits>
streamsize RangedStreambuf<charT, traits>::showmanyc()
{
	return min(backend.in_avail(), maxWrite-curReadPos);
}


template <class charT, class traits>
typename basic_streambuf<charT, traits>::int_type
RangedStreambuf<charT, traits>::uflow()
{
	if (curReadPos >= maxWrite) {
		return traits::eof();
	}

	int_type retval = backend.sgetc();

	if (!traits::eq_int_type(retval, traits::eof())) {
		curReadPos++;
		backend.sbumpc();
	}

	return retval;
}


template <class charT, class traits>
typename basic_streambuf<charT, traits>::int_type
RangedStreambuf<charT, traits>::underflow()
{
	if (curReadPos >= maxWrite) {
		return traits::eof();
	}

	return backend.sgetc();
}


template <class charT, class traits>
streamsize RangedStreambuf<charT, traits>::xsgetn(char_type* s, streamsize n)
{
	n = min(n, maxWrite-curReadPos);
	n = backend.sgetn(s, n);
	curReadPos += n;
	return n;
}

#endif /* RANGEDSTREAMBUF_H_ */
