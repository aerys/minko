#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("../Depth/Depth.function.glsl")

uniform vec4        diffuseColor;
uniform sampler2D	frontFaceNormalMap;
uniform sampler2D	backFaceNormalMap;
uniform sampler2D	depthMap;
uniform sampler2D	noiseMap;
uniform vec3 		cameraPosition;
uniform float		time;

varying vec3 vertexPosition;
varying vec3 vertexNormal;
varying vec2 vertexUV;
varying vec3 screenPosition;

vec4
computePixelColorFromNormalMap(sampler2D normalMap, sampler2D depthMap, vec2 screenPositionUV, vec4 color, int useDepthMap)
{
	vec4 diffuse		= vec4(0.0f, 0.0f, 0.0f, 0.0f);
	float highLimit		= 0.98f;
	float depthLimit	= 1.0f;
	
	// init offsets
	vec2 centerOffset		= vec2(0.0f, 0.0f) / MAP_RESOLUTION;
	vec2 leftOffset			= vec2(0.0f, 1.0f) / MAP_RESOLUTION;
	vec2 topLeftOffset		= vec2(1.0f, 1.0f) / MAP_RESOLUTION;
	vec2 topOffset			= vec2(1.0f, 0.0f) / MAP_RESOLUTION;
	vec2 topRightOffset		= vec2(1.0f, -1.0f) / MAP_RESOLUTION;
	vec2 rightOffset		= vec2(0.0f, -1.0f) / MAP_RESOLUTION;
	vec2 bottomRightOffset	= vec2(-1.0f, -1.0f) / MAP_RESOLUTION;
	vec2 bottomOffset		= vec2(-1.0f, 0.0f) / MAP_RESOLUTION;
	vec2 bottomLeftOffset	= vec2(-1.0f, 1.0f) / MAP_RESOLUTION;

	// sample normal texture
	vec3 normalColor		= texture2D(normalMap, screenPositionUV).xyz;
	vec3 upNormalColor		= texture2D(normalMap, screenPositionUV + topRightOffset).xyz;
	vec3 downNormalColor	= texture2D(normalMap, screenPositionUV + topLeftOffset).xyz;
	vec3 rightNormalColor	= texture2D(normalMap, screenPositionUV + bottomRightOffset).xyz;
	vec3 leftNormalColor	= texture2D(normalMap, screenPositionUV + bottomLeftOffset).xyz;
	
	// sample depth 
	float depthCenter		= unpack(texture2D(depthMap, screenPositionUV)) * 100.0f;
	float depthTopRight		= unpack(texture2D(depthMap, screenPositionUV + rightOffset)) * 100.0f;
	float depthBottomRight	= unpack(texture2D(depthMap, screenPositionUV + leftOffset)) * 100.0f;
	float depthBottom		= unpack(texture2D(depthMap, screenPositionUV + bottomOffset)) * 100.0f;
	float depthBottomLeft	= unpack(texture2D(depthMap, screenPositionUV + topOffset)) * 100.0f;

	if (depthTopRight == 0.0f)
		depthTopRight = 1000.0f;

	if (depthBottomRight == 0.0f)
		depthBottomRight = 1000.0f;
	
	if (depthBottom == 0.0f)
		depthBottom = 1000.0f;
	
	if (depthBottomLeft == 0.0f)
		depthBottomLeft = 1000.0f;

	// get normals
	vec3 normal			= normalize((normalColor - 0.5f) * 2.0f);
	vec3 upNormal		= normalize((upNormalColor - 0.5f) * 2.0f);
	vec3 downNormal		= normalize((downNormalColor - 0.5f) * 2.0f);
	vec3 rightNormal	= normalize((rightNormalColor - 0.5f) * 2.0f);
	vec3 leftNormal		= normalize((leftNormalColor - 0.5f) * 2.0f);
	
	vec4 dots = vec4(dot(normal, upNormal), dot(normal, downNormal), dot(normal, rightNormal), dot(normal, leftNormal));
	
	if (dots.x >= 0.0f && dots.x <= highLimit ||
		dots.y >= 0.0f && dots.y <= highLimit ||
		dots.z >= 0.0f && dots.z <= highLimit ||
		dots.w >= 0.0f && dots.w <= highLimit ||
		((abs(depthCenter - depthTopRight)		>= depthLimit) && (useDepthMap == 1)) ||
		((abs(depthCenter - depthBottomRight)	>= depthLimit) && (useDepthMap == 1))  ||
		((abs(depthCenter - depthBottom)		>= depthLimit) && (useDepthMap == 1))  ||
		((abs(depthCenter - depthBottomLeft)	>= depthLimit) && (useDepthMap == 1)) )
		diffuse = vec4(color.rgb, 0.8f);
	else
		diffuse = vec4(color.rgb, 0.1f);

	return diffuse;
}

void main(void)
{
	vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec2 screenPositionUV = screenPosition.xy * 0.5f + 0.5f;

	#ifdef FRONT_FACE_NORMAL_MAP
		diffuse = computePixelColorFromNormalMap(frontFaceNormalMap, depthMap, screenPositionUV, diffuseColor, 1);
	#endif

	#ifdef BACK_FACE_NORMAL_MAP
		vec4 computedColor = computePixelColorFromNormalMap(backFaceNormalMap, depthMap, screenPositionUV, diffuseColor, 0);

		if (computedColor.a > diffuse.a)
			diffuse = vec4(computedColor.rgb, computedColor.a * 0.4f);
	#endif

	float yFract = fract(vertexPosition.y - time * 100.0f);

	if (diffuse.a < 0.5f)
	{
		//diffuse.rgb = mix(diffuse.rgb, vec3(1.0f, 1.0f, 1.0f), 0.1f);
		diffuse.a += pow(sin(vertexPosition.y - time / 1000.0f), 6) * 0.2f;
	}

	float yFract2 = fract(fract(vertexPosition.y - time / 100000.0f) * 8.0f);

	if (diffuse.a < 0.5f && (yFract2 >= 0.1f && yFract2 < 0.2f ||
		yFract2 >= 0.3f && yFract2 < 0.4f ||
		yFract2 >= 0.5f && yFract2 < 0.6f ||
		yFract2 >= 0.7f && yFract2 < 0.8f ||
		yFract2 >= 0.9f))
		diffuse = vec4(diffuse.rgb * 0.7f, diffuse.a);

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
