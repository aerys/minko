#ifdef VERTEX_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "../Skinning.function.glsl"
#pragma include "../Pop.function.glsl"

attribute vec3 aPosition;
attribute vec2 aUV;
attribute vec3 aNormal;

#ifdef VERTEX_UV1
attribute vec2 aUV1;
#endif

#ifdef SKINNING_NUM_BONES
# if SKINNING_NUM_BONES != 0
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
# endif
#endif

#ifdef VERTEX_POP_PROTECTED
attribute float aPopProtected;
#endif

uniform mat4 uModelToWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uWorldToScreenMatrix;

#ifdef UV_SCALE
uniform vec2 uUVScale;
#endif
#ifdef UV_OFFSET
uniform vec2 uUVOffset;
#endif

#ifdef POP_LOD_ENABLED
uniform float uPopLod;
#ifdef POP_BLENDING_ENABLED
uniform float uPopBlendingLod;
#endif
uniform float uPopFullPrecisionLod;
uniform vec3 uPopMinBound;
uniform vec3 uPopMaxBound;
#endif

void main(void)
{ 
    // // Calculate where the vertex ought to be.  This line is equivalent
    // // to the transformations in the CelVertexShader.
    // vec4 original = uProjectionMatrix * (uViewMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0));
    // //vec4 original = vec4(aPosition, 1.0) * uModelToWorldMatrix * uViewMatrix * uProjectionMatrix;
    // //vec4 original = mul(mul(mul(aPosition, uModelToWorldMatrix), View), Projection);
 
    // // Calculates the normal of the vertex like it ought to be.
    // vec4 normal = uProjectionMatrix * (uViewMatrix * uModelToWorldMatrix * vec4(aNormal, 1.0));
    // //vec4 normal = vec4(aNormal, 1.0) * uModelToWorldMatrix * uViewMatrix * uProjectionMatrix;
 
    // // Take the correct "original" location and translate the vertex a little
    // // bit in the direction of the normal to draw a slightly expanded object.
    // // Later, we will draw over most of this with the right color, except the expanded
    // // part, which will leave the outline that we want.
    // vec4 lineThickness = vec4(1);
    // vec4 position = original + (normal * lineThickness);
 
    // gl_Position = position;

    vec3 lineThickness = vec3(6.0);
    vec3 position = aPosition - (aNormal * lineThickness);
	vec4 pos = vec4(position, 1.0);

	#ifdef SKINNING_NUM_BONES
        #if SKINNING_NUM_BONES != 0
		  pos = skinning_moveVertex(pos, aBoneWeightsA, aBoneWeightsB);
        #endif
	#endif

    #ifdef POP_LOD_ENABLED
        float popProtected = 0.0;

        #ifdef VERTEX_POP_PROTECTED
            popProtected = aPopProtected;
        #endif // VERTEX_POP_PROTECTED

        #ifdef POP_BLENDING_ENABLED
            pos = pop_blend(pos, vec3(0.0), uPopLod, uPopBlendingLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
        #else
            pos = pop_quantize(pos, vec3(0.0), uPopLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
        #endif // POP_BLENDING_ENABLED
    #endif // POP_LOD_ENABLED

	#ifdef MODEL_TO_WORLD
		pos = uModelToWorldMatrix * pos;
	#endif

    vec4 screenPos = uWorldToScreenMatrix * pos;

	gl_Position = screenPos;
}

#endif // VERTEX_SHADER
