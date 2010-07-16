/*
 * TextureSearchDialog.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#include "TextureSearchDialog.h"
#include "../../ProfileManager.h"
#include "../../Profile.h"
#include <qstring.h>
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include <qstringlist.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include "../../System.h"



TextureSearchDialog::TextureSearchDialog(QWidget* parent, const File* root)
		: QDialog(parent), rootFile(root)
{
	ui.setupUi(this);
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(onSearch(bool)));
	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));
}


void TextureSearchDialog::onCancel(bool checked)
{
	close();
}


void TextureSearchDialog::onSearch(bool checked)
{
	QString texName = ui.nameField->text();
	QString txdName = ui.txdField->text();
	bool regex = ui.regexBox->isChecked();
	bool caseSensitive = ui.caseBox->isChecked();
	bool exact = ui.exactBox->isChecked();

	StringMatcher::flags flags = 0;

	if (!caseSensitive) {
		flags |= STRING_MATCHER_CASEINSENSITIVE;
	}
	if (regex) {
		flags |= STRING_MATCHER_REGEX;
	}
	if (exact) {
		flags |= STRING_MATCHER_EXACTMATCH;
	}

	StringMatcher texMatcher(texName, flags);
	StringMatcher* txdMatcher;

	if (txdName.isEmpty()) {
		txdMatcher = NULL;
	} else {
		txdMatcher = new StringMatcher(txdName, flags);
	}

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();
	Profile::ResourceIterator it;

	QList<TextureMatch*> results;

	if (rootFile) {
		collectSearchResults(*rootFile, &texMatcher, txdMatcher, results);
	} else {
		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
			File* resource = *it;
			collectSearchResults(*resource, &texMatcher, txdMatcher, results);
		}
	}

	bool closeDialog = false;

	if (results.size() == 0) {
		QMessageBox::information(this, tr("No Match"), tr("No texture matching your criteria was found!"));
	} else if (results.size() == 1) {
		System::getInstance()->openFile(*results.at(0)->txdFile);
		closeDialog = true;
	} else {
		QStringList userItems;

		QList<TextureMatch*>::iterator rit;
		for (rit = results.begin() ; rit != results.end() ; rit++) {
			TextureMatch* match = *rit;
			userItems << tr("%1 in %2").arg(match->textureName).arg(match->txdFile->getPath()->toString());
		}

		bool okSelected;
		QString sel = QInputDialog::getItem(this, tr("Multiple Matches"), tr("Please select the texture you want to open:"),
				userItems, 0, false, &okSelected);

		if (okSelected) {
			TextureMatch* match = results.at(userItems.indexOf(sel));
			System::getInstance()->openFile(*match->txdFile);
			closeDialog = true;
		}
	}

	QList<TextureMatch*>::iterator rit;
	for (rit = results.begin() ; rit != results.end() ; rit++) {
		TextureMatch* match = *rit;
		delete match->txdFile;
		delete match;
	}

	if (closeDialog) {
		close();
	}
}


void TextureSearchDialog::collectSearchResults(const File& resource, StringMatcher* texMatcher, StringMatcher* txdMatcher,
			QList<TextureMatch*>& results)
{
	if (resource.isDirectory()  ||  resource.isArchiveFile()) {
		FileIterator* it = resource.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			collectSearchResults(*child, texMatcher, txdMatcher, results);
			delete child;
		}

		delete it;
	} else {
		if (resource.guessContentType() == CONTENT_TYPE_TXD) {
			QString fname = resource.getPath()->getFileName();

			if (!txdMatcher  ||  txdMatcher->matches(fname)) {
				TXDArchive txd(resource);
				TXDTexture* tex;

				for (int i = 0 ; i < txd.getTextureCount() ; i++) {
					tex = txd.nextTexture();

					if (texMatcher->matches(tex->getDiffuseName())) {
						TextureMatch* match = new TextureMatch;
						match->txdFile = new File(resource);
						match->textureName = tex->getDiffuseName();

						results << match;
						delete tex;
						break;
					}

					delete tex;
				}
			}
		}
	}
}

