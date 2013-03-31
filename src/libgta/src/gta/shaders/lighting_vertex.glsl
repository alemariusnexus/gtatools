/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

struct LightSource
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
	vec3 direction;
	float cutoffAngleCos;
	float shininess;
	float spotlightExponent;
	float constAttenuation;
	float linearAttenuation;
	float quadAttenuation;
};

uniform vec4 MaterialEmission;

uniform vec4 GlobalAmbientLight;

uniform vec4 MaterialAmbientReflection;
uniform vec4 MaterialDiffuseReflection;
uniform vec4 MaterialSpecularReflection;

uniform vec3 CameraPosition;

uniform LightSource LightSources[10];

uniform bool DynamicLightingEnabled;




void DirectionalLight (
		inout vec4 color,
		vec3 vertex,
		vec3 normal,
		int i
) {
	color += LightSources[i].ambient * MaterialAmbientReflection;
	
	float dirDotNormal = dot(LightSources[i].direction, normal);
	
	float diffuseFactor = max(dirDotNormal, 0.0);
	color += diffuseFactor * LightSources[i].diffuse * MaterialDiffuseReflection;
	
	if (dirDotNormal > 0.0) {
		vec3 s = normalize(LightSources[i].direction + normalize(CameraPosition - vertex));
		float specularFactor = pow(max(dot(s, normal), 0.0), LightSources[i].shininess);
		color += specularFactor * LightSources[i].specular * MaterialSpecularReflection;
	}
}


void PointLight (
		inout vec4 color,
		vec3 vertex,
		vec3 normal,
		int i
) {
	vec3 vertex2pos = LightSources[i].position - vertex;
	vec3 direction = normalize(vertex2pos);
	float dist = length(vertex2pos);
	float att = 1.0 / (
			LightSources[i].constAttenuation
			+ LightSources[i].linearAttenuation*dist
			+ LightSources[i].quadAttenuation*dist*dist);

	color += LightSources[i].ambient * MaterialAmbientReflection * att;
	
	float dirDotNormal = dot(direction, normal);
	
	float diffuseFactor = max(dirDotNormal, 0.0);
	color += diffuseFactor * LightSources[i].diffuse * MaterialDiffuseReflection * att;
	
	if (dirDotNormal > 0.0) {
		vec3 s = normalize(direction + normalize(CameraPosition - vertex));
		float specularFactor = pow(max(dot(s, normal), 0.0), LightSources[i].shininess);
		color += specularFactor * LightSources[i].specular * MaterialSpecularReflection * att;
	}
}


void SpotLight (
		inout vec4 color,
		vec3 vertex,
		vec3 normal,
		int i
) {
	vec3 light2vertex = vertex - LightSources[i].position;
	vec3 nLight2vertex = normalize(light2vertex);
	
	float cosAngle = max(dot(nLight2vertex, -LightSources[i].direction), 0.0);
	
	if (cosAngle >= LightSources[i].cutoffAngleCos) {
		float spotFactor = pow(cosAngle, LightSources[i].spotlightExponent);
		
		vec3 vertex2pos = LightSources[i].position - vertex;
		vec3 direction = normalize(vertex2pos);
		float dist = length(vertex2pos);
		float att = 1.0 / (
				LightSources[i].constAttenuation
				+ LightSources[i].linearAttenuation*dist
				+ LightSources[i].quadAttenuation*dist*dist
				);
		att *= spotFactor;
	
		color += LightSources[i].ambient * MaterialAmbientReflection * att;
		
		float dirDotNormal = dot(direction, normal);
		
		float diffuseFactor = max(dirDotNormal, 0.0);
		color += diffuseFactor * LightSources[i].diffuse * MaterialDiffuseReflection * att;
		
		if (dirDotNormal > 0.0) {
			vec3 s = normalize(LightSources[i].direction + normalize(CameraPosition - vertex));
			float specularFactor = pow(max(dot(s, normal), 0.0), LightSources[i].shininess);
			color += specularFactor * LightSources[i].specular * MaterialSpecularReflection * att;
		}
	}
}


void CalculateVertexLighting(inout vec4 color, vec3 eVertex, vec3 eNormal)
{
	vec4 globalAmbient = GlobalAmbientLight * MaterialAmbientReflection;
    
    vec4 lightContrib = vec4(0.0, 0.0, 0.0, 0.0);
    
    if (DynamicLightingEnabled) {
    	for (int i = 0 ; i < 10 ; i++) {
	    	if (LightSources[i].cutoffAngleCos <= -2.5) {
	    		break;
	    	} else if (LightSources[i].cutoffAngleCos <= -1.5) {
	    		PointLight(lightContrib, eVertex, eNormal, i);
	    	} else if (LightSources[i].cutoffAngleCos <= -0.5) {
	    		DirectionalLight(lightContrib, eVertex, eNormal, i);
	    	} else {
	    		SpotLight(lightContrib, eVertex, eNormal, i);
	    	}
	    }
	}
    
    color += globalAmbient + lightContrib;
}
