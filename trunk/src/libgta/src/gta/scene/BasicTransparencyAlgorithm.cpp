/*
 * BasicTransparencyAlgorithm.cpp
 *
 *  Created on: 22.09.2011
 *      Author: alemariusnexus
 */

#include "BasicTransparencyAlgorithm.h"


#include <res_glsl110_vertex_default_shader.h>
#include <res_glsl110_fragment_default_shader.h>

#include <res_glsl140_vertex_default_shader.h>
#include <res_glsl140_fragment_default_shader.h>

#include <res_glsles2_vertex_default_shader.h>
#include <res_glsles2_fragment_default_shader.h>



void BasicTransparencyAlgorithm::install()
{
	if (!inited) {
		const char* vertexDefaultData;
		const char* fragmentDefaultData;

		int vertexDefaultDataLen;
		int fragmentDefaultDataLen;

#ifdef GTA_USE_OPENGL_ES
		vertexDefaultData					= (const char*) res_glsles2_vertex_default_shader_data;
		fragmentDefaultData					= (const char*) res_glsles2_fragment_default_shader_data;

		vertexDefaultDataLen					= sizeof(res_glsles2_vertex_default_shader_data);
		fragmentDefaultDataLen					= sizeof(res_glsles2_fragment_default_shader_data);
#else
		if (gtaglIsVersionSupported(3, 1)) {
			vertexDefaultData					= (const char*) res_glsl140_vertex_default_shader_data;
			fragmentDefaultData					= (const char*) res_glsl140_fragment_default_shader_data;

			vertexDefaultDataLen					= sizeof(res_glsl140_vertex_default_shader_data);
			fragmentDefaultDataLen					= sizeof(res_glsl140_fragment_default_shader_data);
		} else {
			vertexDefaultData					= (const char*) res_glsl110_vertex_default_shader_data;
			fragmentDefaultData					= (const char*) res_glsl110_fragment_default_shader_data;

			vertexDefaultDataLen					= sizeof(res_glsl110_vertex_default_shader_data);
			fragmentDefaultDataLen					= sizeof(res_glsl110_fragment_default_shader_data);
		}
#endif

		vertexDefaultShader = new Shader(GL_VERTEX_SHADER, "Basic Transparency Algorithm Vertex Shader");
		vertexDefaultShader->loadSourceCode(vertexDefaultData, vertexDefaultDataLen);
		vertexDefaultShader->compile();

		fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER,
				"Basic Transparency Algorithm Fragment Shader");
		fragmentDefaultShader->loadSourceCode(fragmentDefaultData, fragmentDefaultDataLen);
		fragmentDefaultShader->compile();

		inited = true;
	}
}


void BasicTransparencyAlgorithm::installShaders(ShaderProgram* program)
{
	program->attachShader(vertexDefaultShader);
	program->attachShader(fragmentDefaultShader);
}


void BasicTransparencyAlgorithm::uninstallShaders(ShaderProgram* program)
{
	program->detachShader(vertexDefaultShader);
	program->detachShader(fragmentDefaultShader);
}
