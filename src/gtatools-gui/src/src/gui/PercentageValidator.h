#ifndef PERCENTAGEVALIDATOR_H_
#define PERCENTAGEVALIDATOR_H_

#include <QtGui/QDoubleValidator>


class PercentageValidator : public QDoubleValidator
{
	Q_OBJECT

public:
	PercentageValidator(QObject* parent = NULL) : QDoubleValidator(parent) {}

	virtual QValidator::State validate(QString& input, int& pos) const
	{
		QString trimmedInput = input.trimmed();
		if (input.endsWith('%')) {
			trimmedInput = trimmedInput.left(trimmedInput.length()-1);
		}
		int tpos = pos;
		QValidator::State state = QDoubleValidator::validate(trimmedInput, tpos);

		input = trimmedInput + "%";

		return state;
	}
};

#endif /* PERCENTAGEVALIDATOR_H_ */
