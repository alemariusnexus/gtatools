/*
 * TextureSearchDialog.h
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#ifndef TEXTURESEARCHDIALOG_H_
#define TEXTURESEARCHDIALOG_H_

#include <ui_TextureSearchDialog.h>
#include <qdialog.h>
#include "../../StringMatcher.h"
#include <qlinkedlist.h>
#include <gtaformats/util/File.h>




class TextureSearchDialog : public QDialog {
	Q_OBJECT

private:
	struct TextureMatch {
		File* txdFile;
		QString textureName;
	};

public:
	TextureSearchDialog(QWidget* parent, const File* root = NULL);

private:
	void collectSearchResults(const File& resource, StringMatcher* texMatcher, StringMatcher* txdMatcher,
			QList<TextureMatch*>& results);

private slots:
	void onCancel(bool checked);
	void onSearch(bool checked);

private:
	Ui_TextureSearchDialog ui;
	const File* rootFile;
};

#endif /* TEXTURESEARCHDIALOG_H_ */
