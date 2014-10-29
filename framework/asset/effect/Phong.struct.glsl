#ifdef FRAGMENT_SHADER

struct AmbientLight
{
	vec3 	color;
	float 	ambient;
};

struct DirectionalLight
{
	vec3 	color;
	float 	diffuse;
	float 	specular;
	vec3 	direction;
};

struct PointLight
{
	vec3	color;
	float	diffuse;
	float	specular;
	vec3	attenuationCoeffs;
	vec3	position;
};

struct SpotLight
{
	vec3	direction;
	float	diffuse;
	vec3	position;
	float	cosInnerConeAngle;
	vec3	color;
	float	cosOuterConeAngle;
	vec3	attenuationCoeffs;
	float	specular;
};

#endif // FRAGMENT_SHADER
