#ifndef FIXEDSIZELINEEDIT_H_
#define FIXEDSIZELINEEDIT_H_

#include <QtGui/QLineEdit>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>


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
