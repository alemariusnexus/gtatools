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

#ifndef RANGEDSTREAM_H_
#define RANGEDSTREAM_H_

#include "RangedStreambuf.h"
#include <ios>


template <class StreamClass>
class RangedStream : public StreamClass {
public:
	typedef typename StreamClass::char_type char_type;
	typedef typename StreamClass::traits_type traits_type;
	typedef RangedStreambuf<char_type, traits_type> BufType;

public:
	RangedStream(StreamClass* backend, streamsize endOffset, bool autoDeleteBackend = false);
	//RangedStream(typename BufType::BackType& backend, streamsize endOffset);
	~RangedStream();

private:
	bool autoDeleteBackend;
	StreamClass* backend;
};



template <class StreamClass>
RangedStream<StreamClass>::RangedStream(StreamClass* backend, streamsize endOffset, bool autoDeleteBackend)
		: StreamClass(new BufType(*backend->rdbuf(), endOffset)), autoDeleteBackend(autoDeleteBackend),
		  backend(backend)
{
}


/*template <class StreamClass>
RangedStream<StreamClass>::RangedStream(typename BufType::BackType& backend, streamsize endOffset)
		: StreamClass(new BufType(backend, endOffset)), autoDeleteBackend(true)
{
}*/


template <class StreamClass>
RangedStream<StreamClass>::~RangedStream()
{
	delete this->rdbuf();
	if (autoDeleteBackend)
		delete backend;
}


#endif /* RANGEDSTREAM_H_ */
