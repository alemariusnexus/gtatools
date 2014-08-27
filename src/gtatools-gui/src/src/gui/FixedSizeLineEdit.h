/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef FIXEDSIZELINEEDIT_H_
#define FIXEDSIZELINEEDIT_H_

#include <QLineEdit>
#include <QFont>
#include <QFontMetrics>


class FixedSizeLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	FixedSizeLineEdit(QWidget* parent = NULL)
			: QLineEdit(parent), sizeInChars(-1)
	{
		calculateTextSize();
	}

	void setSizeInCharacters(int sizeInChars)
	{
		this->sizeInChars = sizeInChars;
		calculateTextSize();
	}

	unsigned int getSizeInCharacters() const
	{
		return sizeInChars;
	}

	void setSizeAlphabet(const QString& alphabet)
	{
		sizeAlphabet = alphabet;
		calculateTextSize();
	}

	virtual QSize sizeHint() const
	{
		if (font() != calculatedSizeFont)
			const_cast<FixedSizeLineEdit*>(this)->calculateTextSize();

		QSize size = QLineEdit::sizeHint();

		if (sizeInChars < 0)
			return size;

		QFontMetrics fm(font());

		// Note the small "safety" margin
		size.setWidth(textSize + minimumSizeHint().width() + 5);

		return size;
	}

private:
	void calculateTextSize()
	{
		QFont f = font();
		calculatedSizeFont = f;

		QFontMetrics fm(f);

		textSize = 0;

		if (sizeAlphabet.isEmpty()) {
			textSize = fm.maxWidth();
		} else {
			for (QString::iterator it = sizeAlphabet.begin() ; it != sizeAlphabet.end() ; it++) {
				QChar c = *it;
				textSize = qMax(fm.width(c), textSize);
			}
		}

		textSize = textSize * sizeInChars + fm.leading();
	}

private:
	int sizeInChars;
	QString sizeAlphabet;
	int textSize;
	QFont calculatedSizeFont;
};

#endif /* FIXEDSIZELINEEDIT_H_ */
