/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef DEFAULTRENDERER_H_
#define DEFAULTRENDERER_H_

#include "Renderer.h"
#include "RenderingMesh.h"
#include "AdvancedShaderProgram.h"
#include "AdvancedShader.h"
#include "ShaderPlugin.h"
#include "ShaderPluginRegistry.h"
#include "../Shader.h"
#include "../ShaderProgram.h"
#include "../scene/objects/LightSource.h"
#include <nxcommon/ResourceCache.h>
#include <list>
#include <set>
#include <map>

using std::list;
using std::set;
using std::multimap;



class DefaultRenderer : public Renderer
{
private:
	enum AdvancedShaderProgramID
	{
		OpaqueStaticShaderProgram = 0,
		TransparentStaticShaderProgram = 1,
		OpaqueAnimatedShaderProgram = 2,
		TransparentAnimatedShaderProgram = 3,
		//DepthPeelingBlendLayerShaderProgram = 4,
		//DepthPeelingBlendFinalShaderProgram = 5,

		AdvancedShaderProgramIDCount
	};

	class AdvancedShaderProgramCacheLess
	{
	public:
		bool operator()(const ShaderPluginRegistry& r1, const ShaderPluginRegistry& r2) const { return r1 < r2; }
	};

	class AdvancedShaderProgramCacheEqual
	{
	public:
		bool operator()(const ShaderPluginRegistry& r1, const ShaderPluginRegistry& r2) const { return r1 == r2; }
	};

	class AdvancedShaderProgramCacheHash
	{
	public:
		size_t operator()(const ShaderPluginRegistry& r) const { return r.getHash(); }
	};

private:
	typedef ResourceCache<ShaderPluginRegistry,
			AdvancedShaderProgramCacheLess,
			AdvancedShaderProgramCacheHash,
			AdvancedShaderProgramCacheEqual>
			AdvancedShaderProgramCache;

	struct StaticRenderingMeshEntry
	{
		RenderingMesh* mesh;
		uint32_t id;
	};

	struct AnimatedRenderingMeshEntry
	{
		RenderingMesh* mesh;
		uint32_t id;
	};

private:
	/*typedef list<StaticRenderingMesh*> StaticMeshList;
	typedef StaticMeshList::iterator StaticMeshIterator;
	typedef StaticMeshList::const_iterator ConstStaticMeshIterator;

	typedef list<AnimatedRenderingMesh*> AnimatedMeshList;
	typedef AnimatedMeshList::iterator AnimatedMeshIterator;
	typedef AnimatedMeshList::const_iterator ConstAnimatedMeshIterator;*/

	typedef map<AdvancedShaderProgramCache::Pointer, list<StaticRenderingMeshEntry> > StaticMeshMap;
	typedef StaticMeshMap::iterator StaticMeshIterator;
	typedef StaticMeshMap::const_iterator ConstStaticMeshIterator;

	typedef map<AdvancedShaderProgramCache::Pointer, list<AnimatedRenderingMeshEntry> > AnimatedMeshMap;
	typedef AnimatedMeshMap::iterator AnimatedMeshIterator;
	typedef AnimatedMeshMap::const_iterator ConstAnimatedMeshIterator;

	typedef list<LightSource*> LightSourceList;
	typedef LightSourceList::iterator LightSourceIterator;
	typedef LightSourceList::const_iterator ConstLightSourceIterator;

private:
	class AdvancedShaderProgramCacheEntry : public AdvancedShaderProgramCache::Entry
	{
	public:
		AdvancedShaderProgramCacheEntry(DefaultRenderer* renderer) : renderer(renderer) {}
		virtual ~AdvancedShaderProgramCacheEntry()
		{
			for (size_t i = 0 ; i < AdvancedShaderProgramIDCount ; i++) {
				renderer->spLocs.erase(programs[i]);
				delete programs[i];
			}
		}

		virtual cachesize_t getSize() const { return 1; }

	public:
		AdvancedShaderProgram* programs[AdvancedShaderProgramIDCount];

	private:
		DefaultRenderer* renderer;
	};

	class AdvancedShaderProgramCacheLoader : public AdvancedShaderProgramCache::EntryLoader
	{
	public:
		AdvancedShaderProgramCacheLoader(DefaultRenderer* renderer) : renderer(renderer) {}
		virtual AdvancedShaderProgramCache::Entry* load(ShaderPluginRegistry r)
		{
			AdvancedShaderProgramCacheEntry* entry = renderer->buildShaderPrograms(r);
			return entry;
		}

	private:
		DefaultRenderer* renderer;
	};

private:
	struct LightSourceUniformLocations
	{
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint position;
		GLint direction;
		GLint cutoffAngleCos;
		GLint shininess;
		GLint spotlightExponent;
		GLint constAttenuation;
		GLint linearAttenuation;
		GLint quadAttenuation;
	};

	struct ShaderProgramLocations
	{
		GLint camPosUniform;
		GLint mMatrixUniform;
		GLint vMatrixUniform;
		GLint vpMatrixUniform;
		GLint mvMatrixUniform;
		GLint mvpMatrixUniform;
		GLint normalMvMatrixUniform;
		GLint texturedUniform;
		GLint materialColorUniform;
		GLint vertexColorsUniform;
		GLint textureUniform;

		GLint matAmbientReflectionUniform;
		GLint matDiffuseReflectionUniform;
		GLint matSpecularReflectionUniform;
		GLint globalAmbientLightUniform;
		GLint dynamicLightingEnabledUniform;

		GLint boneMatUniform;
		GLint boneUniform;

		GLint boneMatSizeUniform;

		GLint submeshOffsetUniform;

		GLint vertexAttrib;
		GLint normalAttrib;
		GLint colorAttrib;
		GLint texCoordAttrib;
		GLint boneWeightAttrib;
		GLint boneIndexAttrib;

		GLuint matrixUBIndex;

		LightSourceUniformLocations lightSources[10];
	};

public:
	DefaultRenderer();
	virtual ~DefaultRenderer();

	//virtual void enqueueForRendering(RenderingEntity* entity);
	//virtual void enqueueForRendering(LightSource* ls);

	virtual void enqueueForRendering(list<RenderingEntity*>::iterator beg, list<RenderingEntity*>::iterator end);
	virtual void enqueueForRendering(list<LightSource*>::iterator beg, list<LightSource*>::iterator end);

	virtual void render();

	virtual ShaderPluginRegistry& getDepthPeelingBlendLayerPluginRegistry() { return dpBlendLayerPluginRegistry; }
	virtual const ShaderPluginRegistry& getDepthPeelingBlendLayerPluginRegistry() const { return dpBlendLayerPluginRegistry; }
	virtual void setDepthPeelingBlendLayerPluginRegistry(const ShaderPluginRegistry& reg) { dpBlendLayerPluginRegistry = reg; }

	virtual ShaderPluginRegistry& getDepthPeelingBlendFinalPluginRegistry() { return dpBlendFinalPluginRegistry; }
	virtual const ShaderPluginRegistry& getDepthPeelingBlendFinalPluginRegistry() const { return dpBlendFinalPluginRegistry; }
	virtual void setDepthPeelingBlendFinalPluginRegistry(const ShaderPluginRegistry& reg) { dpBlendFinalPluginRegistry = reg; }

private:
	void setupShaders();
	void setupFBOs();
	void setupBuffers();

	void destroyShaders();
	void destroyShaderPrograms();
	void destroyFBOs();
	void destroyBuffers();

	void initializeUniforms(AdvancedShaderProgram* program);
	void setupDynamicLighting(bool enabled);
	void dpSetupShaderUniforms(AdvancedShaderProgram* program);
	void bindFixedShaderProgramLocations(AdvancedShaderProgram* program);
	void applyShaderPluginUniformBuffers(AdvancedShaderProgram* program);

	void renderShadowVolumes(LightSource* light) {}

	void renderStaticMeshes(list<StaticRenderingMeshEntry>::iterator beg, list<StaticRenderingMeshEntry>::iterator end);
	void renderAnimatedMeshes(list<AnimatedRenderingMeshEntry>::iterator beg, list<AnimatedRenderingMeshEntry>::iterator end);

	AdvancedShaderProgramCacheEntry* buildShaderPrograms(const ShaderPluginRegistry& pluginReg);
	void updateMiscShaderPrograms();

private:
	StaticMeshMap opaqueStaticMeshes;
	StaticMeshMap transStaticMeshes;
	AnimatedMeshMap opaqueAnimMeshes;
	AnimatedMeshMap transAnimMeshes;

	list<RenderingEntity*> renderingEntityDisposalList;

	LightSourceList lightSources;

	struct {
		AdvancedShader* shadeVertexShader;
		AdvancedShader* shadeFragmentShader;
		AdvancedShader* vertexDefaultShader;
		AdvancedShader* fragmentDefaultShader;
		AdvancedShader* animShadeVertexShader;
		AdvancedShader* animShadeFragmentShader;
		AdvancedShader* lightingVertexShader;
		/*AdvancedShader* svVertexShader;
		AdvancedShader* svFragmentShader;
		AdvancedShader* svBlendVertexShader;
		AdvancedShader* svBlendFragmentShader;*/

		AdvancedShader* dpPeelLayerVertexShader;
		AdvancedShader* dpPeelLayerFragmentShader;
		AdvancedShader* dpBlendLayerVertexShader;
		AdvancedShader* dpBlendLayerFragmentShader;
		AdvancedShader* dpBlendFinalVertexShader;
		AdvancedShader* dpBlendFinalFragmentShader;
	} shaders;

	struct {
		/*AdvancedShaderProgram* svProgram;
		AdvancedShaderProgram* svBlendProgram;*/

		AdvancedShaderProgram* dpBlendLayerProgram;
		AdvancedShaderProgram* dpBlendFinalProgram;
	} programs;

	ShaderPluginRegistry dpBlendLayerLinkedPluginRegistry;
	ShaderPluginRegistry dpBlendFinalLinkedPluginRegistry;

	ShaderPluginRegistry dpBlendLayerPluginRegistry;
	ShaderPluginRegistry dpBlendFinalPluginRegistry;

	struct {
		GLuint dpFBO;
		GLuint dpPingPongFBOs[2];
		GLuint dpOpaqueFBO;

		GLuint dpColorTex;
		GLuint dpPingPongDepthStencilTexes[2];
		GLuint dpPingPongColorTexes[2];
		GLuint dpOpaqueTexes[2];

		GLuint dpRBO;
		GLuint dpStencilRBO;
	} fbos;

	struct {
		GLint dpBlendLayerTexUniform, dpBlendFinalTexUniform;
		GLint dpBlendLayerVertexAttrib, dpBlendFinalVertexAttrib;
	} dpShaderLocations;


	GLuint planeDataBuf;

	GLint maxVertexUniformComponents;

	map<AdvancedShaderProgram*, ShaderProgramLocations> spLocs;
	ShaderProgramLocations* renderProgramLocations;

	bool globalAmbientLightEnabled;
	LightSourceIterator enabledLightBegin, enabledLightEnd;

	Matrix4 vMatrix, pMatrix, vpMatrix, vRotationMatrix;

	AdvancedShaderProgramCache* programCache;
	//ShaderPlugin* testPlugin;

	size_t numObjects;

	size_t currentMatrixAllocSize;
	Matrix4* mvMatrices;
	Matrix4* mvpMatrices;
	Matrix4* normalMatrices;

	AdvancedShaderProgram* currentProgram;

	GLuint dummyTex;

public:
	int dpNumPasses;
};

#endif /* DEFAULTRENDERER_H_ */
