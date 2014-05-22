#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("../Fog.function.glsl")

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform sampler2D	frontFaceNormalMap;
uniform sampler2D	backFaceNormalMap;
uniform sampler2D	noiseMap;
uniform samplerCube	diffuseCubeMap;
uniform vec3 		cameraPosition;
// alpha
uniform sampler2D 	alphaMap;
uniform float 		alphaThreshold;
uniform float		time;

varying vec3 vertexPosition;
varying vec3 vertexNormal;
varying vec2 vertexUV;
varying vec3 screenPosition;

vec4
computePixelColorFromNormalMap(sampler2D normalMap, vec2 screenPositionUV, vec4 color)
{
	vec4 diffuse = vec4(0.0, 0.0, 0.0, 0.0);
	float width = 2048.0;
	float height = 2048.0;
	float highLimit = 0.8;
	
	vec3 normalColor		= texture2D(normalMap, screenPositionUV).xyz;
	vec3 upNormalColor		= texture2D(normalMap, screenPositionUV + vec2(0.0, -1.0 / width)).xyz;
	vec3 downNormalColor	= texture2D(normalMap, screenPositionUV + vec2(0.0, 1.0 / width)).xyz;
	vec3 rightNormalColor	= texture2D(normalMap, screenPositionUV + vec2(-1.0 / height, 0.0)).xyz;
	vec3 leftNormalColor	= texture2D(normalMap, screenPositionUV + vec2(1.0 / height, 0.0)).xyz;
	
	vec3 upNormalColor2		= texture2D(normalMap, screenPositionUV + vec2(0.0, -2.0 / width)).xyz;
	vec3 downNormalColor2	= texture2D(normalMap, screenPositionUV + vec2(0.0, 2.0 / width)).xyz;
	vec3 rightNormalColor2	= texture2D(normalMap, screenPositionUV + vec2(-2.0 / height, 0.0)).xyz;
	vec3 leftNormalColor2	= texture2D(normalMap, screenPositionUV + vec2(2.0 / height, 0.0)).xyz;
	
	vec3 upNormalColor3		= texture2D(normalMap, screenPositionUV + vec2(1.0 / height, 1.0 / width)).xyz;
	vec3 downNormalColor3	= texture2D(normalMap, screenPositionUV + vec2(-1.0/ height, 1.0 / width)).xyz;
	vec3 rightNormalColor3	= texture2D(normalMap, screenPositionUV + vec2(1.0 / height, -1.0 / width)).xyz;
	vec3 leftNormalColor3	= texture2D(normalMap, screenPositionUV + vec2(-1.0 / height, -1.0 / width)).xyz;
	
	vec3 normal			= normalize((normalColor - 0.5) * 2.0);
	vec3 upNormal		= normalize((upNormalColor - 0.5) * 2.0);
	vec3 downNormal		= normalize((downNormalColor - 0.5) * 2.0);
	vec3 rightNormal	= normalize((rightNormalColor - 0.5) * 2.0);
	vec3 leftNormal		= normalize((leftNormalColor - 0.5) * 2.0);
	
	vec3 upNormal2		= normalize((upNormalColor2 - 0.5) * 2.0);
	vec3 downNormal2	= normalize((downNormalColor2 - 0.5) * 2.0);
	vec3 rightNormal2	= normalize((rightNormalColor2 - 0.5) * 2.0);
	vec3 leftNormal2	= normalize((leftNormalColor2 - 0.5) * 2.0);
	
	vec3 upNormal3		= normalize((upNormalColor3 - 0.5) * 2.0);
	vec3 downNormal3	= normalize((downNormalColor3 - 0.5) * 2.0);
	vec3 rightNormal3	= normalize((rightNormalColor3 - 0.5) * 2.0);
	vec3 leftNormal3	= normalize((leftNormalColor3 - 0.5) * 2.0);
	
	float upNormalDot		= dot(normal, upNormal);
	float downNormalDot		= dot(normal, downNormal);
	float rightNormalDot	= dot(normal, rightNormal);
	float leftNormalDot		= dot(normal, leftNormal);
	
	float upNormalDot2		= dot(normal, upNormal2);
	float downNormalDot2	= dot(normal, downNormal2);
	float rightNormalDot2	= dot(normal, rightNormal2);
	float leftNormalDot2	= dot(normal, leftNormal2);
	
	float upNormalDot3		= dot(normal, upNormal3);
	float downNormalDot3	= dot(normal, downNormal3);
	float rightNormalDot3	= dot(normal, rightNormal3);
	float leftNormalDot3	= dot(normal, leftNormal3);
	
	if (upNormalDot >= 0 && upNormalDot <= highLimit ||
		downNormalDot >= 0 && downNormalDot <= highLimit ||
		rightNormalDot >= 0 && rightNormalDot <= highLimit ||
		leftNormalDot >= 0 && leftNormalDot <= highLimit || 
		upNormalColor == vec3(0.0, 0.0, 0.0) ||
		downNormalColor == vec3(0.0, 0.0, 0.0) ||
		rightNormalColor == vec3(0.0, 0.0, 0.0) ||
		leftNormalColor == vec3(0.0, 0.0, 0.0) ||
		upNormalDot2 >= 0 && upNormalDot2 <= highLimit ||
		downNormalDot2 >= 0 && downNormalDot2 <= highLimit ||
		rightNormalDot2 >= 0 && rightNormalDot2 <= highLimit ||
		leftNormalDot2 >= 0 && leftNormalDot2 <= highLimit || 
		upNormalColor2 == vec3(0.0, 0.0, 0.0) ||
		downNormalColor2 == vec3(0.0, 0.0, 0.0) ||
		rightNormalColor2 == vec3(0.0, 0.0, 0.0) ||
		leftNormalColor2 == vec3(0.0, 0.0, 0.0)||
		upNormalDot3 >= 0 && upNormalDot3 <= highLimit ||
		downNormalDot3 >= 0 && downNormalDot3 <= highLimit ||
		rightNormalDot3 >= 0 && rightNormalDot3 <= highLimit ||
		leftNormalDot3 >= 0 && leftNormalDot3 <= highLimit || 
		upNormalColor3 == vec3(0.0, 0.0, 0.0) ||
		downNormalColor3 == vec3(0.0, 0.0, 0.0) ||
		rightNormalColor3 == vec3(0.0, 0.0, 0.0) ||
		leftNormalColor3 == vec3(0.0, 0.0, 0.0))
		diffuse = color;
	else
		diffuse = vec4(color.rgb, 0.075);
	
	return diffuse;
}

void main(void)
{
	vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
	vec2 screenPositionUV = screenPosition.xy * 0.5 + 0.5;

	#ifdef FRONT_FACE_NORMAL_MAP
		diffuse = computePixelColorFromNormalMap(frontFaceNormalMap, screenPositionUV, diffuseColor);
	#endif

	#ifdef BACK_FACE_NORMAL_MAP
		vec4 computedColor = computePixelColorFromNormalMap(backFaceNormalMap, screenPositionUV, diffuseColor);

		if (computedColor.a > diffuse.a)
			diffuse = computedColor * 0.8;
	#endif

	float yFract = fract(vertexPosition.y - time / 1900.0);

	if (yFract >= 0.15 && yFract < 0.21)
		diffuse = vec4(diffuse.rgb * 0.6, diffuse.a);

	#ifdef NOISE_MAP
		float noise = texture2D(noiseMap, screenPositionUV * 8.0 - vec2(0.0, time / 250.0)).x;

		noise = clamp(noise * 3.0, 0.0, 1.0);
		if (diffuse.a < 0.2)
			diffuse.a *= noise;

	#endif

	float yFract2 = fract(fract(vertexPosition.y - time / 100000.0) * 8.0);

	if (yFract2 >= 0.1 && yFract2 < 0.2 ||
		yFract2 >= 0.3 && yFract2 < 0.4 ||
		yFract2 >= 0.5 && yFract2 < 0.6 ||
		yFract2 >= 0.7 && yFract2 < 0.8 ||
		yFract2 >= 0.9)
		diffuse = vec4(diffuse.rgb * 0.8, diffuse.a);

	gl_FragColor = diffuse;

	//gl_FragColor = texture2D(backFaceNormalMap, vertexUV);
}

#endif // FRAGMENT_SHADER
