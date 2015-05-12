#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "../Math.function.glsl"

#if defined(CUBE_MAP)
    uniform samplerCube uCubeMap;
#elif defined(LATLONG_MAP)
    uniform sampler2D uLatLongMap;
#endif

#ifdef GAMMA_CORRECTION
    uniform float uGammaCorrection;
#endif

varying vec3 vDirection;

void main()
{
    vec4 color = vec4(0.0);

    #if defined(CUBE_MAP)
        color = textureCube(uCubeMap, normalize(vDirection));
    #elif defined(LATLONG_MAP)
        color = texture2D(uLatLongMap, normalToLatLongUV(normalize(vDirection)));
    #endif

    #if defined(GAMMA_CORRECTION)
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    #endif

    gl_FragColor = color;
}
