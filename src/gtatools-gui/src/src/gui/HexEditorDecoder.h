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

#ifndef HEXEDITORDECODER_H_
#define HEXEDITORDECODER_H_

#include <gtatools-gui/config.h>
#include <ui_HexEditorDecoder.h>
#include <QtGui/QWidget>
#include "HexEditor.h"
#include <QtCore/QtEndian>
#include <nxcommon/util.h>



class HexEditorDecoder : public QWidget {
	Q_OBJECT

public:
	HexEditorDecoder(QWidget* parent = NULL);
	void connectToEditor(HexEditor* editor);
	void setData(const QByteArray& data) { this->data = data.left(8); }
	void setEditable(bool editable);
	bool isEditable() const { return editable; }

signals:
	void dataChanged(const QByteArray& data);

private:
	void display();
	uint16_t convertEndian(uint16_t val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndian16(val) : ToBigEndian16(val); }
	int16_t convertEndian(int16_t val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndian16(val) : ToBigEndian16(val); }
	uint32_t convertEndian(uint32_t val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndian32(val) : ToBigEndian32(val); }
	int32_t convertEndian(int32_t val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndian32(val) : ToBigEndian32(val); }
	uint64_t convertEndian(uint64_t val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndian64(val) : ToBigEndian64(val); }
	int64_t convertEndian(int64_t val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndian64(val) : ToBigEndian64(val); }
	float convertEndian(float val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndianF32(val) : ToBigEndianF32(val); }
	double convertEndian(double val) const
			{ return ui.littleEndianBox->isChecked() ? ToLittleEndianF64(val) : ToBigEndianF64(val); }

private slots:
	void editorSelDataChanged();
	void endiannessToggled(bool little);
	void editedSigned8();
	void editedSigned16();
	void editedSigned32();
	void editedSigned64();
	void editedUnsigned8();
	void editedUnsigned16();
	void editedUnsigned32();
	void editedUnsigned64();
	void editedFloat32();
	void editedFloat64();
	void editedBinary();

private:
	Ui_HexEditorDecoder ui;
	QByteArray data;
	bool editable;
};

#endif /* HEXEDITORDECODER_H_ */
