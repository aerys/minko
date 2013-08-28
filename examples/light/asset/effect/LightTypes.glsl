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
	vec3 position;
};

struct SpotLight
{
	vec3 color;
	float diffuse;
	float specular;
	vec3 position;
	vec3 direction;
	float cosInnerConeAngle;
	float cosOuterConeAngle;
};