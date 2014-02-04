#ifdef GL_ES
	precision mediump float;
#endif

float
blinnPhong_roughnessToShininess(float roughness)
{
	float m = max(0.01, min(1.0, roughness));
	
	return 2.0 * (1.0 / (m * m) - 1.0);
}

vec3 
blinnPhong_Fresnel(vec3 specularColor, float HdotL)
{
	float k		= 1.0 - HdotL;
	float kk	= k * k;
	
	// Schlick's approximation
	return specularColor + (vec3(1.0) - specularColor) * kk * kk * k;
}

float 
blinnPhong_NDF(float NdotH, float shininess)
{
	return (shininess + 2.0) * pow(NdotH, shininess) * 0.159154943; // approximation of BRDF normalization constant : (a + 2) / (2 * PI)
}

float 
blinnPhong_CookTorranceGeometricTerm(float NdotV, float NdotL, float NdotH, float VdotH)
{
	return min(1.0, 2.0 * NdotH * min(NdotV, NdotL) / VdotH);
}

vec3 
blinnPhong_specularReflection(vec3	normal,
							  vec3	lightVector,
							  vec3	viewVector,
							  float	shininess,
							  vec3	specularColor)
{
	float	NdotV		= dot(normal, viewVector);
	if (NdotV < 0.0)
		return vec3(0.0);
	
	vec3	halfVector	= normalize(lightVector + viewVector);
	float	NdotL		= max(0.0, dot(normal,		lightVector));
	float	NdotH		= max(0.0, dot(normal, 		halfVector));
	float	HdotL		= max(0.0, dot(halfVector,	lightVector));
	float	VdotH		= max(0.0, dot(viewVector,	halfVector));
	
	return blinnPhong_Fresnel(specularColor, HdotL)
		* blinnPhong_NDF(NdotH, shininess) 
		* blinnPhong_CookTorranceGeometricTerm(NdotV, NdotL, NdotH, VdotH)
		* 0.25;
}