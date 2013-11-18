#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#ifndef MINKO_NO_GLSL_STRUCT

	struct AmbientLight
	{
		vec3 color;
		float ambient;	
	};

	struct DirectionalLight
	{
		vec3 color;
		float diffuse;
		float specular;
		vec3 direction;	
	};

	struct PointLight
	{
		vec3 color;
		float diffuse;
		float specular;
		float attenuationDistance;
		vec3 position;
	};

	struct SpotLight
	{
		vec3 color;
		float diffuse;
		float specular;
		float attenuationDistance;
		vec3 position;
		vec3 direction;
		float cosInnerConeAngle;
		float cosOuterConeAngle;
	};

#endif // 	MINKO_NO_GLSL_STRUCT

#endif // FRAGMENT_SHADER