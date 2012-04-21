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

#ifndef INT64VALIDATOR_H_
#define INT64VALIDATOR_H_

#include <gtatools-gui/config.h>
#include <QtGui/QValidator>


class Int64Validator : public QValidator {
	Q_OBJECT

public:
	Int64Validator(int64_t min, int64_t max, QObject* parent = NULL)
			: QValidator(parent), vmin(min), vmax(max) {}
	Int64Validator(QObject* parent = NULL) : QValidator(parent), vmin(INT64_MIN), vmax(INT64_MAX) {}
	virtual State validate(QString& input, int& pos) const;

private:
	int64_t vmin, vmax;
};

#endif /* INT64VALIDATOR_H_ */
