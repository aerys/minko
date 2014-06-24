#ifdef FRAGMENT_SHADER

#pragma include('../Phong.function.glsl')

uniform sampler2D normalMap;

varying vec2 vertexUV;
varying vec3 worldNormal;
varying vec3 worldTangent;

void main(void)
{
	vec3 normal = normalize(worldNormal);

	#ifdef NORMAL_MAP
		mat3 tangentToWorldMatrix 	= phong_getTangentToWorldSpaceMatrix(normal, normalize(worldTangent));
			
		normal = tangentToWorldMatrix * normalize(2.0f * texture2D(normalMap, vertexUV).xyz - 1.0f);
	#endif

	gl_FragColor = vec4((normalize(normal) + 1.0f) * 0.5f, 1.0f);
}

#endif
