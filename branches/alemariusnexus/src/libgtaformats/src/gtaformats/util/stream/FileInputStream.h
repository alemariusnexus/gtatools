/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef FILEINPUTSTREAM_H_
#define FILEINPUTSTREAM_H_

#include "STLInputStream.h"
#include "../File.h"


#define STREAM_BINARY (1<<0)



/**	\brief InputStream reading from files.
 */
class FileInputStream : public STLInputStream {
public:
	/**	\brief Constructs a new FileInputStream.
	 *
	 * 	@param file The file to read from.
	 * 	@param flags The read flags. Can be 0 or STREAM_BINARY.
	 */
	FileInputStream(const File& file, int flags = 0);

	/**	\brief Constructs a new FileInputStream.
	 *
	 * 	@param file The file to read from.
	 * 	@param flags The read flags. Can be 0 or STREAM_BINARY.
	 */
	FileInputStream(const char* file, int flags = 0);

private:
	/**	\brief Initialized this stream.
	 */
	void init(const File& file, int flags);
};

#endif /* FILEINPUTSTREAM_H_ */
