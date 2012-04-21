/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef UINT64VALIDATOR_H_
#define UINT64VALIDATOR_H_

#include <gtatools-gui/config.h>
#include <QtGui/QValidator>


class UInt64Validator : public QValidator {
	Q_OBJECT

public:
	UInt64Validator(uint64_t min, uint64_t max, QObject* parent = NULL)
			: QValidator(parent), vmin(min), vmax(max) {}
	UInt64Validator(QObject* parent = NULL) : QValidator(parent), vmin(0), vmax(UINT64_MAX) {}
	virtual State validate(QString& input, int& pos) const;

private:
	uint64_t vmin, vmax;
};

#endif /* UINT64VALIDATOR_H_ */
