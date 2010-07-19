/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

	int numFiles = 0;
	int filesDone = 0;

	if (rootFile) {
		numFiles = rootFile->getChildCount(true);
	} else {
		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
			File* resource = *it;
			numFiles += resource->getChildCount(true);
		}
	}

	/*QProgressDialog pd(tr("Searching texture..."), tr("&Cancel"), 0, numFiles, this);
	pd.setMinimumDuration(1000);
	pd.setWindowModality(Qt::WindowModal);*/

	System* sys = System::getInstance();
	sys->startTask(0, 100, tr("Searching texture..."));

	if (rootFile) {
		collectSearchResults(*rootFile, &texMatcher, txdMatcher, results, numFiles, filesDone);
	} else {
		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
			File* resource = *it;
			collectSearchResults(*resource, &texMatcher, txdMatcher, results, numFiles, filesDone);
		}
	}

	sys->endTask();

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
			QList<TextureMatch*>& results, int filesMax, int& filesDone)
{
	if (resource.isDirectory()  ||  resource.isArchiveFile()) {
		FileIterator* it = resource.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			collectSearchResults(*child, texMatcher, txdMatcher, results, filesMax, filesDone);
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

	filesDone++;

	if (filesDone%(filesMax/100) == 0) {
		System* sys = System::getInstance();
		sys->updateTaskValue(sys->getTaskValue()+1);
	}
}

