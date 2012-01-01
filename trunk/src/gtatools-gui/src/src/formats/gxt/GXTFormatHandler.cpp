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

#include "GXTFormatHandler.h"
#include "GXTWidget.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include "../../DefaultDisplayedFile.h"
#include <gtaformats/util/Exception.h>
#include "../../System.h"



GXTFormatHandler* GXTFormatHandler::getInstance()
{
	static GXTFormatHandler inst;
	return &inst;
}


bool GXTFormatHandler::hasFileFormat(const File& file) const
{
	return QString(file.getPath()->getExtension()).toLower().compare("gxt") == 0;
}


DisplayedFile* GXTFormatHandler::openFile(const FileOpenRequest& request)
{
	GXTWidget* widget;

	try {
		widget = new GXTWidget(request, NULL);
	} catch (Exception& ex) {
		System::getInstance()->log(LogEntry::error(tr("Error opening GXT file: %1").arg(ex.getMessage()),
				&ex));
		return NULL;
	}

	DefaultDisplayedFile* file = new DefaultDisplayedFile(*request.getFile(), this, widget);
	return file;
}


void GXTFormatHandler::iniExport(const File& file, const QLinkedList<GXTTable*>& tables)
{
	QFile qfile(file.getPath()->toString());

	if (qfile.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream stream(&qfile);
		stream.setCodec("UTF-8");
		QLinkedList<GXTTable*>::const_iterator it;

		for (it = tables.begin() ; it != tables.end() ; it++) {
			GXTTable* table = *it;
			stream << "[" << table->getName() << "]\n";

			GXTTable::EntryIterator it;

			for (it = table->getFirstEntry() ; it != table->getLastEntry() ; it++) {
				crc32_t keyHash = it->first;
				QString value = QString::fromUtf8(it->second);
				const char* key = table->getKeyName(keyHash);

				if (key) {
					stream << key << "=" << value << "\n";
				} else {
					QString keyHashStr = QString("%1").arg((uint32_t) keyHash, 8, 16, QLatin1Char('0'));
					stream << keyHashStr.toUpper() << "=" << value << "\n";
				}
			}

			stream << "\n";
		}

		stream.flush();
	}
}


void GXTFormatHandler::stringListMatch(const File& matchFile, const QMap<QString, GXTTable*>& tables,
		QWidget* parent)
{
	istream* stream = matchFile.openInputStream();

	GXTTable* currentTable = NULL;

	char buffer[4096];
	while (!stream->eof()) {
		stream->getline(buffer, sizeof(buffer));

		QString line(buffer);
		int cmtIndex = line.indexOf("//");

		if (cmtIndex != -1) {
			line = line.left(cmtIndex);
		}

		line = line.trimmed();

		if (line.isEmpty()) {
			continue;
		}

		if (line.startsWith('[')  &&  line.endsWith(']')) {
			QString tableName = line.mid(1, line.length()-2);
			//langTables = &tables[tableName];

			currentTable = tables[tableName];
		} else {
			if (!currentTable) {
				QMessageBox::critical(parent, tr("Syntax Error"),
						tr("Syntax Error: Key name definition is not in a table!"));
				return;
			}

			QStringList kv = line.split('=');
			crc32_t keyHash;
			QString keyName;

			if (kv.size() == 1) {
				keyName = line;
				keyHash = Crc32(line.toAscii().constData());
			} else {
				keyHash = kv[0].toUInt(NULL, 16);
				keyName = kv[1];
			}

			currentTable->setKeyName(keyHash, keyName.toAscii().constData());
		}
	}

	delete stream;
}
