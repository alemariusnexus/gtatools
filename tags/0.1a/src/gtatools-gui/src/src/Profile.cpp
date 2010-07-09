/*
 * Profile.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "Profile.h"
#include <gtaformats/engine.h>
#include <wx/filename.h>
#include <wx/dir.h>



Profile::Profile(const wxString& name)
		: name(wxString(name))
{
}


void Profile::addResource(const wxString& resource)
{
	wxString* resCpy = new wxString(resource);
	resourceList.push_back(resCpy);
}


void addResourcesToIndex(ResourceIndex* resIdx, const wxString& path)
{
	if (wxFileName::FileExists(path)) {
		const char* cRes = path->mb_str();
		resIdx->addResource(cRes);
	} else if (wxFileName::DirExists(path)) {
		wxDir dir(res);

		wxString* child;
		if (dir.GetFirst(child)) {
			do {
				addResourcesToIndex(resIdx, child);
			} while (dir.GetNext(child));
		}
	}
}


ResourceIndex* Profile::createResourceIndex()
{
	ResourceIndex* resIdx = new ResourceIndex;

	vector<wxString*>::iterator it;

	for (it = resourceList.begin() ; it != resourceList.end() ; it++) {
		wxString* res = *it;
		addResourcesToIndex(resIdx, *res);
	}

	return resIdx;
}


