#ifdef FRAGMENT_SHADER

#pragma include "../TextureLod.extension.glsl"

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "../TextureLod.function.glsl"
#pragma include "../LightMapping.function.glsl"

uniform vec4 uDiffuseColor;

#ifdef DIFFUSE_MAP
uniform sampler2D uDiffuseMap;
#endif

#ifdef LIGHT_MAP
uniform sampler2D uLightMap;
#endif

uniform sampler2D uDiscretizedLightMap;

#ifdef DIFFUSE_CUBEMAP
uniform samplerCube uDiffuseCubeMap;
#endif

// alpha
#ifdef ALPHA_MAP
uniform sampler2D uAlphaMap;
#endif

#ifdef ALPHA_THRESHOLD
uniform float uAlphaThreshold;
#endif

// texture lod
#ifdef DIFFUSE_MAP_LOD
uniform float uDiffuseMapMaxAvailableLod;
uniform vec2 uDiffuseMapSize;
#endif

#ifdef LIGHT_MAP_LOD
uniform float uLightMapMaxAvailableLod;
uniform vec2 uLightMapSize;
#endif

# if NUM_DIRECTIONAL_LIGHTS > 0
uniform vec3 uDirLight0_direction;
uniform vec3 uDirLight0_color;
uniform float uDirLight0_diffuse;
# endif

uniform vec3 uCameraDirection;
uniform vec3 uCameraPosition;
uniform float uBorderThreshold;

varying vec2 vVertexUV;
varying vec2 vVertexUV1;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;
varying vec3 vVertexNormal;
varying vec3 vVertexPosition;

void main(void)
{
    vec4 diffuse = uDiffuseColor;

    #if defined(VERTEX_UV)
        #if defined(DIFFUSE_CUBEMAP)
            diffuse = textureCube(uDiffuseCubeMap, vVertexUVW);
        #elif defined(DIFFUSE_MAP)
            #ifdef DIFFUSE_MAP_LOD
                diffuse = texturelod_texture2D(uDiffuseMap, vVertexUV, uDiffuseMapSize, 0.0, uDiffuseMapMaxAvailableLod, diffuse);
            #else
                diffuse = texture2D(uDiffuseMap, vVertexUV);
            #endif
        #endif
    #endif // VERTEX_UV

    #if defined(VERTEX_UV) && defined(ALPHA_MAP)
        diffuse.a = texture2D(uAlphaMap, vVertexUV).r;
    #endif // VERTEX_UV && ALPHA_MAP

    #ifdef ALPHA_THRESHOLD
        if (diffuse.a < uAlphaThreshold)
            discard;
    #endif // ALPHA_THRESHOLD

    #if (defined (VERTEX_UV) || defined(VERTEX_UV1)) && defined(LIGHT_MAP)
        vec2 lightMapUV = vec2(0.0);

        #ifdef VERTEX_UV1
            lightMapUV = vVertexUV1;
        #else
            lightMapUV = vVertexUV;
        #endif

        #ifdef LIGHT_MAP_LOD
            diffuse = lightMapping_multiply(diffuse, texturelod_texture2D(uLightMap, lightMapUV, uLightMapSize, 0.0, uLightMapMaxAvailableLod, vec4(1.0)));
        #else
            diffuse = lightMapping_multiply(diffuse, texture2D(uLightMap, lightMapUV));
        #endif
    #endif // (VERTEX_UV || VERTEX_UV1) && LIGHT_MAP

    # if NUM_DIRECTIONAL_LIGHTS > 0
        vec3 lightDirection = normalize(-uDirLight0_direction);
        float intensity = dot(lightDirection, vVertexNormal);

        intensity = clamp(intensity, 0.0, 1.0);
        diffuse *= texture2D(uDiscretizedLightMap, vec2(intensity, 0));
    #endif

    vec3 eyeVector = normalize(uCameraPosition - vVertexPosition);
    float powValue = 10.;
    float value = pow(dot(normalize(vVertexNormal), normalize(eyeVector)), powValue);

    // if (value < pow(uBorderThreshold, powValue))
    //     diffuse.rgb = vec3(0.0, 0.0, 0.0);

    //value = (value + 1.0) * 0.5;
    //diffuse.rgb = vec3(value, value, value);
    gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
