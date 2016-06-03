#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Math.function.glsl"
#pragma include "ToneMapping.function.glsl"

#if defined(CUBE_MAP)
    uniform samplerCube uCubeMap;
#elif defined(LATLONG_MAP)
    uniform sampler2D uLatLongMap;
#endif

#ifdef ORIENTATION
    uniform float uOrientation;
#endif

#ifdef BRIGHTNESS
    uniform float uBrightness;
#endif

#ifdef GAMMA_CORRECTION
    uniform float uGammaCorrection;
#endif

#ifdef FOG_COLOR
    uniform vec4 uFogColor;
#endif

varying vec3 vDirection;

void main()
{
    vec4 color = vec4(0.0);

    #if defined(CUBE_MAP)
        color = textureCube(uCubeMap, normalize(vDirection));
    #elif defined(LATLONG_MAP)
        vec2 latLongUV = normalToLatLongUV(normalize(vDirection));

        #ifdef ORIENTATION
            latLongUV.x += uOrientation;
        #endif

        color = texture2D(uLatLongMap, latLongUV);
    #endif

    #if defined(GAMMA_CORRECTION) && (defined(CUBE_MAP) || defined(LATLONG_MAP))
        color.rgb = pow(color.rgb, vec3(uGammaCorrection));
    #endif

    #if defined(BRIGHTNESS)
        color.rgb = toneMapping_toneMap(color.rgb, uBrightness);
    #endif

    #if defined(FOG_COLOR)
        color.rgb = mix(color.rgb, uFogColor.rgb, pow(1.0 - saturate(vDirection.y), 30.0));
    #endif

    #if defined(GAMMA_CORRECTION)
        color.rgb = pow(color.rgb, vec3(1.0 / uGammaCorrection));
    #endif

    gl_FragColor = color;
}
