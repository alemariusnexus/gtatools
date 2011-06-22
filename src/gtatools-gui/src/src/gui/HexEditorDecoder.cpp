/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "HexEditorDecoder.h"
#include <QtGui/QIntValidator>
#include <QtGui/QDoubleValidator>
#include "../Int64Validator.h"
#include "../UInt64Validator.h"
#include <gtaformats/util/util.h>



HexEditorDecoder::HexEditorDecoder(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.littleEndianBox, SIGNAL(toggled(bool)), this, SLOT(endiannessToggled(bool)));

	ui.signed8Field->setValidator(new QIntValidator(INT8_MIN, INT8_MAX, this));
	ui.signed16Field->setValidator(new QIntValidator(INT16_MIN, INT16_MAX, this));
	ui.signed32Field->setValidator(new QIntValidator(INT32_MIN, INT32_MAX, this));
	ui.signed64Field->setValidator(new Int64Validator(this));

	ui.unsigned8Field->setValidator(new QIntValidator(0, UINT8_MAX, this));
	ui.unsigned16Field->setValidator(new QIntValidator(0, UINT16_MAX, this));
	ui.unsigned32Field->setValidator(new UInt64Validator(0, UINT32_MAX, this));
	ui.unsigned64Field->setValidator(new UInt64Validator(this));

	ui.float32Field->setValidator(new QDoubleValidator(this));
	ui.float64Field->setValidator(new QDoubleValidator(this));
	ui.binaryField->setInputMask("Bbbbbbbbb bbbbbbbb bbbbbbbb bbbbbbbb");

	connect(ui.signed8Field, SIGNAL(editingFinished()), this, SLOT(editedSigned8()));
	connect(ui.signed16Field, SIGNAL(editingFinished()), this, SLOT(editedSigned16()));
	connect(ui.signed32Field, SIGNAL(editingFinished()), this, SLOT(editedSigned32()));
	connect(ui.signed64Field, SIGNAL(editingFinished()), this, SLOT(editedSigned64()));

	connect(ui.unsigned8Field, SIGNAL(editingFinished()), this, SLOT(editedUnsigned8()));
	connect(ui.unsigned16Field, SIGNAL(editingFinished()), this, SLOT(editedUnsigned16()));
	connect(ui.unsigned32Field, SIGNAL(editingFinished()), this, SLOT(editedUnsigned32()));
	connect(ui.unsigned64Field, SIGNAL(editingFinished()), this, SLOT(editedUnsigned64()));

	connect(ui.float32Field, SIGNAL(editingFinished()), this, SLOT(editedFloat32()));
	connect(ui.float64Field, SIGNAL(editingFinished()), this, SLOT(editedFloat64()));
	connect(ui.binaryField, SIGNAL(editingFinished()), this, SLOT(editedBinary()));
}


void HexEditorDecoder::connectToEditor(HexEditor* editor)
{
	connect(editor, SIGNAL(cursorChanged(int, int)), this, SLOT(editorSelDataChanged()));
	connect(editor, SIGNAL(dataChanged(const QByteArray&)), this, SLOT(editorSelDataChanged()));
	connect(this, SIGNAL(dataChanged(const QByteArray&)), editor, SLOT(replace(const QByteArray&)));
}


void HexEditorDecoder::editorSelDataChanged()
{
	HexEditor* editor = (HexEditor*) sender();
	setData(editor->getData().mid(editor->getCursorPosition(), 8));
	display();

	ui.offsetLabel->setText(QString("0x%1").arg(QString("%1").arg(editor->getCursorPosition(), 0, 16).toUpper()));
	ui.selectionLabel->setText(QString("0x%1 - 0x%2").arg(QString("%1").arg(editor->getCursorAnchor(), 0, 16).toUpper())
			.arg(QString("%1").arg(editor->getCursorPosition(), 0, 16).toUpper()));
}


void HexEditorDecoder::endiannessToggled(bool little)
{
	display();
}


void HexEditorDecoder::display()
{
	bool littleEndian = ui.littleEndianBox->isChecked();

	int8_t* s8Ptr = (int8_t*) data.constData();
	int16_t* s16Ptr = (int16_t*) data.constData();
	int32_t* s32Ptr = (int32_t*) data.constData();
	int64_t* s64Ptr = (int64_t*) data.constData();

	uint8_t* u8Ptr = (uint8_t*) data.constData();
	uint16_t* u16Ptr = (uint16_t*) data.constData();
	uint32_t* u32Ptr = (uint32_t*) data.constData();
	uint64_t* u64Ptr = (uint64_t*) data.constData();

	float* f32Ptr = (float*) data.constData();
	double* f64Ptr = (double*) data.constData();

	int dsize = data.size();

	ui.signed8Field->setText((dsize >= 1) ? QString("%1").arg(*s8Ptr) : tr("-"));
	ui.signed16Field->setText((dsize >= 2) ? QString("%1").arg(convertEndian(*s16Ptr)) : tr("-"));
	ui.signed32Field->setText((dsize >= 4) ? QString("%1").arg(convertEndian(*s32Ptr)) : tr("-"));
	ui.signed64Field->setText((dsize >= 8) ? QString("%1").arg(convertEndian(*s64Ptr)) : tr("-"));

	ui.unsigned8Field->setText((dsize >= 1) ? QString("%1").arg(*u8Ptr) : tr("-"));
	ui.unsigned16Field->setText((dsize >= 2) ? QString("%1").arg(convertEndian(*u16Ptr)) : tr("-"));
	ui.unsigned32Field->setText((dsize >= 4) ? QString("%1").arg(convertEndian(*u32Ptr)) : tr("-"));
	ui.unsigned64Field->setText((dsize >= 8) ? QString("%1").arg(convertEndian(*u64Ptr)) : tr("-"));

	ui.float32Field->setText((dsize >= 4) ? QString("%1").arg(convertEndian(*f32Ptr)) : tr("-"));
	ui.float64Field->setText((dsize >= 8) ? QString("%1").arg(convertEndian(*f64Ptr)) : tr("-"));

	ui.asciiField->setText(QString("%1").arg(QString(data.left(8))));

	QString binaryText(tr("-"));

	if (dsize >= 1) {
		binaryText = QString("%1").arg(*u8Ptr, 8, 2, QChar('0'));
		ui.binaryField->setInputMask("Bbbbbbbb");
	}
	if (dsize >= 2) {
		binaryText += QString(" %1").arg(*((uint8_t*) u16Ptr + 1), 8, 2, QChar('0'));
		ui.binaryField->setInputMask("Bbbbbbbb bbbbbbbb");
	}
	if (dsize >= 3) {
		binaryText += QString(" %1").arg(*((uint8_t*) u32Ptr + 2), 8, 2, QChar('0'));
		ui.binaryField->setInputMask("Bbbbbbbb bbbbbbbb bbbbbbbb");
	}
	if (dsize >= 4) {
		binaryText += QString(" %1").arg(*((uint8_t*) u32Ptr + 3), 8, 2, QChar('0'));
		ui.binaryField->setInputMask("Bbbbbbbb bbbbbbbb bbbbbbbb bbbbbbbb");
	}

	ui.binaryField->setText(binaryText);

	ui.signed8Field->setEnabled(dsize >= 1);
	ui.signed16Field->setEnabled(dsize >= 2);
	ui.signed32Field->setEnabled(dsize >= 4);
	ui.signed64Field->setEnabled(dsize >= 8);

	ui.unsigned8Field->setEnabled(dsize >= 1);
	ui.unsigned16Field->setEnabled(dsize >= 2);
	ui.unsigned32Field->setEnabled(dsize >= 4);
	ui.unsigned64Field->setEnabled(dsize >= 8);

	ui.float32Field->setEnabled(dsize >= 4);
	ui.float64Field->setEnabled(dsize >= 8);
	ui.binaryField->setEnabled(dsize >= 1);
}


void HexEditorDecoder::editedSigned8()
{
	int8_t value = (int8_t) ui.signed8Field->text().toInt();
	emit dataChanged(QByteArray((char*) &value, 1));
}


void HexEditorDecoder::editedSigned16()
{
	int16_t value = convertEndian((int16_t) ui.signed16Field->text().toInt());
	emit dataChanged(QByteArray((char*) &value, 2));
}


void HexEditorDecoder::editedSigned32()
{
	int32_t value = convertEndian((int32_t) ui.signed32Field->text().toInt());
	emit dataChanged(QByteArray((char*) &value, 4));
}


void HexEditorDecoder::editedSigned64()
{
	int64_t value = convertEndian((int64_t) ui.signed64Field->text().toLongLong());
	emit dataChanged(QByteArray((char*) &value, 8));
}


void HexEditorDecoder::editedUnsigned8()
{
	uint8_t value = (uint8_t) ui.unsigned8Field->text().toUInt();
	emit dataChanged(QByteArray((char*) &value, 1));
}


void HexEditorDecoder::editedUnsigned16()
{
	uint16_t value = convertEndian((uint16_t) ui.unsigned16Field->text().toUInt());
	emit dataChanged(QByteArray((char*) &value, 2));
}


void HexEditorDecoder::editedUnsigned32()
{
	uint32_t value = convertEndian((uint32_t) ui.unsigned32Field->text().toUInt());
	emit dataChanged(QByteArray((char*) &value, 4));
}


void HexEditorDecoder::editedUnsigned64()
{
	uint64_t value = convertEndian((uint64_t) ui.unsigned64Field->text().toULongLong());
	emit dataChanged(QByteArray((char*) &value, 8));
}


void HexEditorDecoder::editedFloat32()
{
	float value = convertEndian((float) ui.float32Field->text().toFloat());
	emit dataChanged(QByteArray((char*) &value, 4));
}


void HexEditorDecoder::editedFloat64()
{
	double value = convertEndian((double) ui.float64Field->text().toDouble());
	emit dataChanged(QByteArray((char*) &value, 8));
}


void HexEditorDecoder::editedBinary()
{
	int dsize = data.size();
	QStringList byteData = ui.binaryField->text().split(' ');
	QByteArray insData;

	if (dsize >= 1)
		insData[0] = (uint8_t) byteData[0].toUShort(NULL, 2);
	if (dsize >= 2)
		insData[1] = (uint8_t) byteData[1].toUShort(NULL, 2);
	if (dsize >= 3)
		insData[2] = (uint8_t) byteData[2].toUShort(NULL, 2);
	if (dsize >= 4)
		insData[3] = (uint8_t) byteData[3].toUShort(NULL, 2);

	emit dataChanged(insData);
}
