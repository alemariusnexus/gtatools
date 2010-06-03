/*
 * Profile.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef PROFILE_H_
#define PROFILE_H_

#include <vector>

using std::vector;
using std::pair;


class Profile {
public:
	Profile(const wxString& name);
	void addResource(const wxString& resource);
	ResourceIndex* createResourceIndex();

private:
	void addResourcesToIndex(ResourceIndex* resIdx, const wxString& path);

private:
	wxString name;
	vector<wxString*> resourceList;
	vector<wxString*> internalResourceList;
};

#endif /* PROFILE_H_ */
