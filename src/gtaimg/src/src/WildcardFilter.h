/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtaimg.

	gtaimg is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaimg is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaimg.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WILDCARDFILTER_H_
#define WILDCARDFILTER_H_

#include "EntryFilter.h"


class WildcardFilter : public EntryFilter {
public:
	WildcardFilter(const char* pattern);
	virtual bool process(const IMGEntry& entry);

private:
	char* pattern;
};

#endif /* WILDCARDFILTER_H_ */
