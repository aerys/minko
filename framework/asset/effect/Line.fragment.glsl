#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

uniform float uTime;
uniform vec4 uDiffuseColor;

#ifdef LINE_DASH_LENGTH
uniform float uLineDashLength;
#ifdef LINE_DASH_SPEED
uniform float uLineDashSpeed;
#endif
#endif

varying float vPosW;
varying float vWeight;

void
main()
{
	vec4 color = vec4(1.0);

	#ifdef DIFFUSE_COLOR
		color = uDiffuseColor;
	#endif // DIFFUSE_COLOR

#ifdef LINE_DASH_LENGTH
    float timeTerm = 0.0;
    #ifdef LINE_DASH_SPEED
        // uTime is in ms
        timeTerm = uLineDashSpeed * uTime / 1000.0;
    #endif

    float dashLength = uLineDashLength * 4.0;
    if (mod(vWeight + timeTerm, dashLength * 2.0) > dashLength)
        color.a = 0.0;

    // color.a *= sign(sin(vWeight / vPosW / uLineDashLength * 1000.0 + timeTerm));
#endif

	gl_FragColor = color;
}

#endif // FRAGMENT_SHADER
