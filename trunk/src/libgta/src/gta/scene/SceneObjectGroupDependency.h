/*
 * SceneObjectGroupDependency.h
 *
 *  Created on: 19.11.2011
 *      Author: alemariusnexus
 */

#ifndef SCENEOBJECTGROUPDEPENDENCY_H_
#define SCENEOBJECTGROUPDEPENDENCY_H_

#include <gta/config.h>
#include <gtaformats/util/CString.h>


class SceneObjectGroupDependency {
public:
	SceneObjectGroupDependency(const CString& relPath);
	CString getRelativePath() const { return relPath; }
	void setModifyTime(uint64_t mtime) { this->mtime = mtime; }
	uint64_t getModifyTime() const { return mtime; }

private:
	CString relPath;
	uint64_t mtime;
};

#endif /* SCENEOBJECTGROUPDEPENDENCY_H_ */
