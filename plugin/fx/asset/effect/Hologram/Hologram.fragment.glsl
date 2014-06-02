#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("../Depth/Depth.function.glsl")

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform sampler2D	frontFaceNormalMap;
uniform sampler2D	backFaceNormalMap;
uniform sampler2D	depthMap;
uniform sampler2D	noiseMap;
uniform samplerCube	diffuseCubeMap;
uniform vec3 		cameraPosition;
// alpha
uniform sampler2D 	alphaMap;
uniform float 		alphaThreshold;
uniform float		time;
uniform float		mapResolution;

varying vec3 vertexPosition;
varying vec3 vertexNormal;
varying vec2 vertexUV;
varying vec3 screenPosition;

vec4
computePixelColorFromNormalMap(sampler2D normalMap, sampler2D depthMap, vec2 screenPositionUV, vec4 color, int useDepthMap)
{
	vec4 diffuse		= vec4(0.0, 0.0, 0.0, 0.0);
	float resolution	= mapResolution;
	float highLimit		= 0.6;
	float depthLimit	= 1.0;
	
	// init offsets
	vec2 centerOffset		= vec2(0.0, 0.0) / resolution;
	vec2 leftOffset			= vec2(0.0, 1.0) / resolution;
	vec2 topLeftOffset		= vec2(1.0, 1.0) / resolution;
	vec2 topOffset			= vec2(1.0, 0.0) / resolution;
	vec2 topRightOffset		= vec2(1.0, -1.0) / resolution;
	vec2 rightOffset		= vec2(0.0, -1.0) / resolution;
	vec2 bottomRightOffset	= vec2(-1.0, -1.0) / resolution;
	vec2 bottomOffset		= vec2(-1.0, 0.0) / resolution;
	vec2 bottomLeftOffset	= vec2(-1.0, 1.0) / resolution;

	// sample normal texture
	vec3 normalColor		= texture2D(normalMap, screenPositionUV).xyz;
	vec3 upNormalColor		= texture2D(normalMap, screenPositionUV + topRightOffset).xyz;
	vec3 downNormalColor	= texture2D(normalMap, screenPositionUV + topLeftOffset).xyz;
	vec3 rightNormalColor	= texture2D(normalMap, screenPositionUV + bottomRightOffset).xyz;
	vec3 leftNormalColor	= texture2D(normalMap, screenPositionUV + bottomLeftOffset).xyz;
	
	// sample depth 
	float depthCenter		= unpack(texture2D(depthMap, screenPositionUV)) * 100.0;
	float depthTopRight		= unpack(texture2D(depthMap, screenPositionUV + rightOffset)) * 100.0;
	float depthBottomRight	= unpack(texture2D(depthMap, screenPositionUV + leftOffset)) * 100.0;
	float depthBottom		= unpack(texture2D(depthMap, screenPositionUV + bottomOffset)) * 100.0;
	float depthBottomLeft	= unpack(texture2D(depthMap, screenPositionUV + topOffset)) * 100.0;

	if (depthTopRight == 0.0)
		depthTopRight = 1000.0;

	if (depthBottomRight == 0.0)
		depthBottomRight = 1000.0;
	
	if (depthBottom == 0.0)
		depthBottom = 1000.0;
	
	if (depthBottomLeft == 0.0)
		depthBottomLeft = 1000.0;

	// get normals
	vec3 normal			= normalize((normalColor - 0.5) * 2.0);
	vec3 upNormal		= normalize((upNormalColor - 0.5) * 2.0);
	vec3 downNormal		= normalize((downNormalColor - 0.5) * 2.0);
	vec3 rightNormal	= normalize((rightNormalColor - 0.5) * 2.0);
	vec3 leftNormal		= normalize((leftNormalColor - 0.5) * 2.0);
	
	vec4 dots = vec4(dot(normal, upNormal), dot(normal, downNormal), dot(normal, rightNormal), dot(normal, leftNormal));
	
	if (dots.x >= 0 && dots.x <= highLimit ||
		dots.y >= 0 && dots.y <= highLimit ||
		dots.z >= 0 && dots.z <= highLimit ||
		dots.w >= 0 && dots.w <= highLimit ||
		((abs(depthCenter - depthTopRight)		>= depthLimit) && (useDepthMap == 1)) ||
		((abs(depthCenter - depthBottomRight)	>= depthLimit) && (useDepthMap == 1))  ||
		((abs(depthCenter - depthBottom)		>= depthLimit) && (useDepthMap == 1))  ||
		((abs(depthCenter - depthBottomLeft)	>= depthLimit) && (useDepthMap == 1)) )
		diffuse = vec4(color.rgb, 1.0);
	else
		diffuse = vec4(color.rgb, 0.09);

	return diffuse;
}

void main(void)
{
	vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
	vec2 screenPositionUV = screenPosition.xy * 0.5 + 0.5;

	#ifdef FRONT_FACE_NORMAL_MAP
		diffuse = computePixelColorFromNormalMap(frontFaceNormalMap, depthMap, screenPositionUV, diffuseColor, 1);
	#endif

	#ifdef BACK_FACE_NORMAL_MAP
		vec4 computedColor = computePixelColorFromNormalMap(backFaceNormalMap, depthMap, screenPositionUV, diffuseColor, 0);

		if (computedColor.a > diffuse.a)
			diffuse = vec4(computedColor.rgb, computedColor.a * 0.6);
	#endif

	float yFract = fract(vertexPosition.y - time / 1900.0);

	if (yFract >= 0.15 && yFract < 0.21 && diffuse.a < 0.5)
		diffuse = vec4(mix(diffuse.rgb, vec3(1.0, 1.0, 1.0), 0.9), diffuse.a * 1.2);

	float yFract2 = fract(fract(vertexPosition.y - time / 100000.0) * 8.0);

	if (diffuse.a < 0.5 && (yFract2 >= 0.1 && yFract2 < 0.2 ||
		yFract2 >= 0.3 && yFract2 < 0.4 ||
		yFract2 >= 0.5 && yFract2 < 0.6 ||
		yFract2 >= 0.7 && yFract2 < 0.8 ||
		yFract2 >= 0.9))
		diffuse = vec4(diffuse.rgb * 0.7, diffuse.a);

	gl_FragColor = diffuse;

	//gl_FragColor = texture2D(backFaceNormalMap, vertexUV);
}

#endif // FRAGMENT_SHADER
