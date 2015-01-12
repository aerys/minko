/************************************************************************************

Filename    :   CAPI_GLE_GL.h
Content     :   GL extensions declarations.
Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/


#ifndef INC_OVR_CAPI_GLE_GL_h
#define INC_OVR_CAPI_GLE_GL_h


#include <stddef.h>


// Windows headers
//     <wingdi.h>            Windows-specific OpenGL 1.1 intefaces. Long ago this was <GL/wgl.h>.
//     <GL/gl.h>             OpenGL 1.1 interface.
//     <GL/glext.h>          OpenGL 1.2+ compatibility profile and extension interfaces. Not provided by Microsoft.
//     <GL/wglext.h>         Windows-specific extension interfaces. Not provided by Microsoft.
//     <GL/glcorearb.h>      OpenGL core profile and ARB extension interfaces. Doesn't include interfaces found only in the compatibility profile. Overlaps with gl.h and glext.h.
//
// Mac headers
//     <OpenGL/gl.h>         OpenGL 1.1 interface.
//     <OpenGL/glext.h>      OpenGL 1.2+ compatibility profile and extension interfaces.
//     <OpenGL/gl3.h>        Includes only interfaces supported in a core OpenGL 3.1 implementations plus a few related extensions.
//     <OpenGL/gl3ext.h>     Includes extensions supproted in a core OpenGL 3.1 implmentation.
//     <OpenGL/OpenGL.h>     Apple-specific extension interfaces.
//
// Linux headers
//     <GL/gl.h>             OpenGL 1.1 interface.
//     <GL/glext.h>          OpenGL 1.2+ compatibility profile and extension interfaces.
//     <GL/glx.h>            X Windows-specific OpenGL interfaces
//     <GL/glxext.h>         X Windows 1.3+ API and GLX extension interfaces.
//     <GL/glcorearb.h>      OpenGL core profile and ARB extension interfaces. Doesn't include interfaces found only in the compatibility profile. Overlaps with gl.h and glext.h.


#if defined(__gl_h_) || defined(__GL_H__) || defined(__X_GL_H)
    #error gl.h should be included after this, not before.
#endif
#if defined(__gl2_h_)
    #error gl2.h should be included after this, not before.
#endif
#if defined(__gltypes_h_)
    #error gltypes.h should be included after this, not before.
#endif
#if defined(__glext_h_) || defined(__GLEXT_H_)
    #error glext.h should be included after this, not before.
#endif

// Prevent other GL versions from being included in the future.
// We do not disable Microsoft's wingdi.h and redeclare its functions. That's a big header that includes many other things.
// We do not currently disable Apple's OpenGL/OpenGL.h, though we could if we replicated its declarations in this header file.
#define __gl_h_             // Disable future #includes of Apple's <OpenGL/gl.h>
#define __GL_H__            // Disable future #includes of Microsoft's <GL/gl.h>
#define __X_GL_H            // etc.
#define __gl2_h_
#define __gltypes_h_
#define __glext_h_
#define __GLEXT_H_


// GLE platform identification
#if defined(_WIN32)
    #define GLE_WINDOWS_ENABLED 1   // WGL interface
#elif defined(__APPLE__)
    #define GLE_APPLE_ENABLED 1     // CGL interface
#elif defined(__ANDROID__)
    #define GLE_ANDROID_ENABLED 1   // EGL interface
#else
    #define GLE_UNIX_ENABLED        // GLX interface
#endif


// GLAPI / GLAPIENTRY
//
// GLAPI is a wrapper for Microsoft __declspec(dllimport).
// GLAPIENTRY is the calling convension (__stdcall under Microsoft).
//
#if defined(GLE_WINDOWS_ENABLED)
    #if !defined(WINAPI)
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN 1
        #endif
        #include <windows.h>
        #undef WIN32_LEAN_AND_MEAN
    #endif

    #ifndef WINGDIAPI // Normally defined via windows.h
        #define WINGDIAPI __declspec(dllimport)
        #define GLE_WINGDIAPI_DEFINED   // We define this so that we can know to undefine WINGDIAPI at the end of this file.
    #endif

    #if !defined(GLAPI)
        #if defined(__MINGW32__) || defined(__CYGWIN__)
            #define GLAPI extern
        #else
            #define GLAPI WINGDIAPI
        #endif
    #endif
    #if !defined(GLAPIENTRY)
        #define GLAPIENTRY __stdcall
    #endif
#else
    #include <stdint.h>

    #define GLAPI      extern
    #define GLAPIENTRY /* empty */
#endif


// GLE_CLASS_EXPORT 
// Possibly maps to Microsoft __declspec(dllexport) or nothing on other platforms.
#define GLE_CLASS_EXPORT // Currently defined to nothing. Could be defined to a dll export type.



// GLE_HOOKING_ENABLED
// When enabled, we intercept all OpenGL calls and do any useful internal processing before or after the call.
// An example use case for this is to intercept OpenGL errors on platforms that don't support the OpenGL 
// debug functionality (e.g. KHR_Debug).
#define GLE_HOOKING_ENABLED 1

// When using hooking, we map all OpenGL function usage to our member functions that end with _Hook. 
// These member hook functions will internally call the actual OpenGL functions after doing some internal processing.
#if defined(GLE_HOOKING_ENABLED)
    #define GLEGetCurrentFunction(x) GLEContext::GetCurrentContext()->x##_Hook
    #define GLEGetCurrentVariable(x) GLEContext::GetCurrentContext()->x
#else
    #define GLEGetCurrentFunction(x) GLEContext::GetCurrentContext()->x##_Impl
    #define GLEGetCurrentVariable(x) GLEContext::GetCurrentContext()->x
#endif



#ifdef __cplusplus
extern "C" {
#endif


// OpenGL 1.1 declarations are present in all versions of gl.h, including Microsoft's.
// You don't need to dynamically link to these functions on any platform and can just assume
// they are present. A number of these functions have been deprecated by OpenGL v3+, and
// if an OpenGL v3+ core profile is enabled then usage of the deprecated functions is an error.

#ifndef GL_VERSION_1_1
    #define GL_VERSION_1_1 1

    typedef unsigned int GLenum;
    typedef unsigned int GLbitfield;
    typedef unsigned int GLuint;
    typedef int GLint;
    typedef int GLsizei;
    typedef unsigned char GLboolean;
    typedef signed char GLbyte;
    typedef short GLshort;
    typedef unsigned char GLubyte;
    typedef unsigned short GLushort;
    typedef unsigned long GLulong;
    typedef float GLfloat;
    typedef float GLclampf;
    typedef double GLdouble;
    typedef double GLclampd;
    typedef void GLvoid;
    typedef int64_t GLint64EXT;
    typedef uint64_t GLuint64EXT;
    typedef GLint64EXT  GLint64;
    typedef GLuint64EXT GLuint64;
    typedef struct __GLsync *GLsync;
    typedef char GLchar;

    #define GL_ZERO 0
    #define GL_FALSE 0
    #define GL_LOGIC_OP 0x0BF1
    #define GL_NONE 0
    #define GL_TEXTURE_COMPONENTS 0x1003
    #define GL_NO_ERROR 0
    #define GL_POINTS 0x0000
    #define GL_CURRENT_BIT 0x00000001
    #define GL_TRUE 1
    #define GL_ONE 1
    #define GL_CLIENT_PIXEL_STORE_BIT 0x00000001
    #define GL_LINES 0x0001
    #define GL_LINE_LOOP 0x0002
    #define GL_POINT_BIT 0x00000002
    #define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
    #define GL_LINE_STRIP 0x0003
    #define GL_LINE_BIT 0x00000004
    #define GL_TRIANGLES 0x0004
    #define GL_TRIANGLE_STRIP 0x0005
    #define GL_TRIANGLE_FAN 0x0006
    #define GL_QUADS 0x0007
    #define GL_QUAD_STRIP 0x0008
    #define GL_POLYGON_BIT 0x00000008
    #define GL_POLYGON 0x0009
    #define GL_POLYGON_STIPPLE_BIT 0x00000010
    #define GL_PIXEL_MODE_BIT 0x00000020
    #define GL_LIGHTING_BIT 0x00000040
    #define GL_FOG_BIT 0x00000080
    #define GL_DEPTH_BUFFER_BIT 0x00000100
    #define GL_ACCUM 0x0100
    #define GL_LOAD 0x0101
    #define GL_RETURN 0x0102
    #define GL_MULT 0x0103
    #define GL_ADD 0x0104
    #define GL_NEVER 0x0200
    #define GL_ACCUM_BUFFER_BIT 0x00000200
    #define GL_LESS 0x0201
    #define GL_EQUAL 0x0202
    #define GL_LEQUAL 0x0203
    #define GL_GREATER 0x0204
    #define GL_NOTEQUAL 0x0205
    #define GL_GEQUAL 0x0206
    #define GL_ALWAYS 0x0207
    #define GL_SRC_COLOR 0x0300
    #define GL_ONE_MINUS_SRC_COLOR 0x0301
    #define GL_SRC_ALPHA 0x0302
    #define GL_ONE_MINUS_SRC_ALPHA 0x0303
    #define GL_DST_ALPHA 0x0304
    #define GL_ONE_MINUS_DST_ALPHA 0x0305
    #define GL_DST_COLOR 0x0306
    #define GL_ONE_MINUS_DST_COLOR 0x0307
    #define GL_SRC_ALPHA_SATURATE 0x0308
    #define GL_STENCIL_BUFFER_BIT 0x00000400
    #define GL_FRONT_LEFT 0x0400
    #define GL_FRONT_RIGHT 0x0401
    #define GL_BACK_LEFT 0x0402
    #define GL_BACK_RIGHT 0x0403
    #define GL_FRONT 0x0404
    #define GL_BACK 0x0405
    #define GL_LEFT 0x0406
    #define GL_RIGHT 0x0407
    #define GL_FRONT_AND_BACK 0x0408
    #define GL_AUX0 0x0409
    #define GL_AUX1 0x040A
    #define GL_AUX2 0x040B
    #define GL_AUX3 0x040C
    #define GL_INVALID_ENUM 0x0500
    #define GL_INVALID_VALUE 0x0501
    #define GL_INVALID_OPERATION 0x0502
    #define GL_STACK_OVERFLOW 0x0503
    #define GL_STACK_UNDERFLOW 0x0504
    #define GL_OUT_OF_MEMORY 0x0505
    #define GL_2D 0x0600
    #define GL_3D 0x0601
    #define GL_3D_COLOR 0x0602
    #define GL_3D_COLOR_TEXTURE 0x0603
    #define GL_4D_COLOR_TEXTURE 0x0604
    #define GL_PASS_THROUGH_TOKEN 0x0700
    #define GL_POINT_TOKEN 0x0701
    #define GL_LINE_TOKEN 0x0702
    #define GL_POLYGON_TOKEN 0x0703
    #define GL_BITMAP_TOKEN 0x0704
    #define GL_DRAW_PIXEL_TOKEN 0x0705
    #define GL_COPY_PIXEL_TOKEN 0x0706
    #define GL_LINE_RESET_TOKEN 0x0707
    #define GL_EXP 0x0800
    #define GL_VIEWPORT_BIT 0x00000800
    #define GL_EXP2 0x0801
    #define GL_CW 0x0900
    #define GL_CCW 0x0901
    #define GL_COEFF 0x0A00
    #define GL_ORDER 0x0A01
    #define GL_DOMAIN 0x0A02
    #define GL_CURRENT_COLOR 0x0B00
    #define GL_CURRENT_INDEX 0x0B01
    #define GL_CURRENT_NORMAL 0x0B02
    #define GL_CURRENT_TEXTURE_COORDS 0x0B03
    #define GL_CURRENT_RASTER_COLOR 0x0B04
    #define GL_CURRENT_RASTER_INDEX 0x0B05
    #define GL_CURRENT_RASTER_TEXTURE_COORDS 0x0B06
    #define GL_CURRENT_RASTER_POSITION 0x0B07
    #define GL_CURRENT_RASTER_POSITION_VALID 0x0B08
    #define GL_CURRENT_RASTER_DISTANCE 0x0B09
    #define GL_POINT_SMOOTH 0x0B10
    #define GL_POINT_SIZE 0x0B11
    #define GL_POINT_SIZE_RANGE 0x0B12
    #define GL_POINT_SIZE_GRANULARITY 0x0B13
    #define GL_LINE_SMOOTH 0x0B20
    #define GL_LINE_WIDTH 0x0B21
    #define GL_LINE_WIDTH_RANGE 0x0B22
    #define GL_LINE_WIDTH_GRANULARITY 0x0B23
    #define GL_LINE_STIPPLE 0x0B24
    #define GL_LINE_STIPPLE_PATTERN 0x0B25
    #define GL_LINE_STIPPLE_REPEAT 0x0B26
    #define GL_LIST_MODE 0x0B30
    #define GL_MAX_LIST_NESTING 0x0B31
    #define GL_LIST_BASE 0x0B32
    #define GL_LIST_INDEX 0x0B33
    #define GL_POLYGON_MODE 0x0B40
    #define GL_POLYGON_SMOOTH 0x0B41
    #define GL_POLYGON_STIPPLE 0x0B42
    #define GL_EDGE_FLAG 0x0B43
    #define GL_CULL_FACE 0x0B44
    #define GL_CULL_FACE_MODE 0x0B45
    #define GL_FRONT_FACE 0x0B46
    #define GL_LIGHTING 0x0B50
    #define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
    #define GL_LIGHT_MODEL_TWO_SIDE 0x0B52
    #define GL_LIGHT_MODEL_AMBIENT 0x0B53
    #define GL_SHADE_MODEL 0x0B54
    #define GL_COLOR_MATERIAL_FACE 0x0B55
    #define GL_COLOR_MATERIAL_PARAMETER 0x0B56
    #define GL_COLOR_MATERIAL 0x0B57
    #define GL_FOG 0x0B60
    #define GL_FOG_INDEX 0x0B61
    #define GL_FOG_DENSITY 0x0B62
    #define GL_FOG_START 0x0B63
    #define GL_FOG_END 0x0B64
    #define GL_FOG_MODE 0x0B65
    #define GL_FOG_COLOR 0x0B66
    #define GL_DEPTH_RANGE 0x0B70
    #define GL_DEPTH_TEST 0x0B71
    #define GL_DEPTH_WRITEMASK 0x0B72
    #define GL_DEPTH_CLEAR_VALUE 0x0B73
    #define GL_DEPTH_FUNC 0x0B74
    #define GL_ACCUM_CLEAR_VALUE 0x0B80
    #define GL_STENCIL_TEST 0x0B90
    #define GL_STENCIL_CLEAR_VALUE 0x0B91
    #define GL_STENCIL_FUNC 0x0B92
    #define GL_STENCIL_VALUE_MASK 0x0B93
    #define GL_STENCIL_FAIL 0x0B94
    #define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
    #define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
    #define GL_STENCIL_REF 0x0B97
    #define GL_STENCIL_WRITEMASK 0x0B98
    #define GL_MATRIX_MODE 0x0BA0
    #define GL_NORMALIZE 0x0BA1
    #define GL_VIEWPORT 0x0BA2
    #define GL_MODELVIEW_STACK_DEPTH 0x0BA3
    #define GL_PROJECTION_STACK_DEPTH 0x0BA4
    #define GL_TEXTURE_STACK_DEPTH 0x0BA5
    #define GL_MODELVIEW_MATRIX 0x0BA6
    #define GL_PROJECTION_MATRIX 0x0BA7
    #define GL_TEXTURE_MATRIX 0x0BA8
    #define GL_ATTRIB_STACK_DEPTH 0x0BB0
    #define GL_CLIENT_ATTRIB_STACK_DEPTH 0x0BB1
    #define GL_ALPHA_TEST 0x0BC0
    #define GL_ALPHA_TEST_FUNC 0x0BC1
    #define GL_ALPHA_TEST_REF 0x0BC2
    #define GL_DITHER 0x0BD0
    #define GL_BLEND_DST 0x0BE0
    #define GL_BLEND_SRC 0x0BE1
    #define GL_BLEND 0x0BE2
    #define GL_LOGIC_OP_MODE 0x0BF0
    #define GL_INDEX_LOGIC_OP 0x0BF1
    #define GL_COLOR_LOGIC_OP 0x0BF2
    #define GL_AUX_BUFFERS 0x0C00
    #define GL_DRAW_BUFFER 0x0C01
    #define GL_READ_BUFFER 0x0C02
    #define GL_SCISSOR_BOX 0x0C10
    #define GL_SCISSOR_TEST 0x0C11
    #define GL_INDEX_CLEAR_VALUE 0x0C20
    #define GL_INDEX_WRITEMASK 0x0C21
    #define GL_COLOR_CLEAR_VALUE 0x0C22
    #define GL_COLOR_WRITEMASK 0x0C23
    #define GL_INDEX_MODE 0x0C30
    #define GL_RGBA_MODE 0x0C31
    #define GL_DOUBLEBUFFER 0x0C32
    #define GL_STEREO 0x0C33
    #define GL_RENDER_MODE 0x0C40
    #define GL_PERSPECTIVE_CORRECTION_HINT 0x0C50
    #define GL_POINT_SMOOTH_HINT 0x0C51
    #define GL_LINE_SMOOTH_HINT 0x0C52
    #define GL_POLYGON_SMOOTH_HINT 0x0C53
    #define GL_FOG_HINT 0x0C54
    #define GL_TEXTURE_GEN_S 0x0C60
    #define GL_TEXTURE_GEN_T 0x0C61
    #define GL_TEXTURE_GEN_R 0x0C62
    #define GL_TEXTURE_GEN_Q 0x0C63
    #define GL_PIXEL_MAP_I_TO_I 0x0C70
    #define GL_PIXEL_MAP_S_TO_S 0x0C71
    #define GL_PIXEL_MAP_I_TO_R 0x0C72
    #define GL_PIXEL_MAP_I_TO_G 0x0C73
    #define GL_PIXEL_MAP_I_TO_B 0x0C74
    #define GL_PIXEL_MAP_I_TO_A 0x0C75
    #define GL_PIXEL_MAP_R_TO_R 0x0C76
    #define GL_PIXEL_MAP_G_TO_G 0x0C77
    #define GL_PIXEL_MAP_B_TO_B 0x0C78
    #define GL_PIXEL_MAP_A_TO_A 0x0C79
    #define GL_PIXEL_MAP_I_TO_I_SIZE 0x0CB0
    #define GL_PIXEL_MAP_S_TO_S_SIZE 0x0CB1
    #define GL_PIXEL_MAP_I_TO_R_SIZE 0x0CB2
    #define GL_PIXEL_MAP_I_TO_G_SIZE 0x0CB3
    #define GL_PIXEL_MAP_I_TO_B_SIZE 0x0CB4
    #define GL_PIXEL_MAP_I_TO_A_SIZE 0x0CB5
    #define GL_PIXEL_MAP_R_TO_R_SIZE 0x0CB6
    #define GL_PIXEL_MAP_G_TO_G_SIZE 0x0CB7
    #define GL_PIXEL_MAP_B_TO_B_SIZE 0x0CB8
    #define GL_PIXEL_MAP_A_TO_A_SIZE 0x0CB9
    #define GL_UNPACK_SWAP_BYTES 0x0CF0
    #define GL_UNPACK_LSB_FIRST 0x0CF1
    #define GL_UNPACK_ROW_LENGTH 0x0CF2
    #define GL_UNPACK_SKIP_ROWS 0x0CF3
    #define GL_UNPACK_SKIP_PIXELS 0x0CF4
    #define GL_UNPACK_ALIGNMENT 0x0CF5
    #define GL_PACK_SWAP_BYTES 0x0D00
    #define GL_PACK_LSB_FIRST 0x0D01
    #define GL_PACK_ROW_LENGTH 0x0D02
    #define GL_PACK_SKIP_ROWS 0x0D03
    #define GL_PACK_SKIP_PIXELS 0x0D04
    #define GL_PACK_ALIGNMENT 0x0D05
    #define GL_MAP_COLOR 0x0D10
    #define GL_MAP_STENCIL 0x0D11
    #define GL_INDEX_SHIFT 0x0D12
    #define GL_INDEX_OFFSET 0x0D13
    #define GL_RED_SCALE 0x0D14
    #define GL_RED_BIAS 0x0D15
    #define GL_ZOOM_X 0x0D16
    #define GL_ZOOM_Y 0x0D17
    #define GL_GREEN_SCALE 0x0D18
    #define GL_GREEN_BIAS 0x0D19
    #define GL_BLUE_SCALE 0x0D1A
    #define GL_BLUE_BIAS 0x0D1B
    #define GL_ALPHA_SCALE 0x0D1C
    #define GL_ALPHA_BIAS 0x0D1D
    #define GL_DEPTH_SCALE 0x0D1E
    #define GL_DEPTH_BIAS 0x0D1F
    #define GL_MAX_EVAL_ORDER 0x0D30
    #define GL_MAX_LIGHTS 0x0D31
    #define GL_MAX_CLIP_PLANES 0x0D32
    #define GL_MAX_TEXTURE_SIZE 0x0D33
    #define GL_MAX_PIXEL_MAP_TABLE 0x0D34
    #define GL_MAX_ATTRIB_STACK_DEPTH 0x0D35
    #define GL_MAX_MODELVIEW_STACK_DEPTH 0x0D36
    #define GL_MAX_NAME_STACK_DEPTH 0x0D37
    #define GL_MAX_PROJECTION_STACK_DEPTH 0x0D38
    #define GL_MAX_TEXTURE_STACK_DEPTH 0x0D39
    #define GL_MAX_VIEWPORT_DIMS 0x0D3A
    #define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH 0x0D3B
    #define GL_SUBPIXEL_BITS 0x0D50
    #define GL_INDEX_BITS 0x0D51
    #define GL_RED_BITS 0x0D52
    #define GL_GREEN_BITS 0x0D53
    #define GL_BLUE_BITS 0x0D54
    #define GL_ALPHA_BITS 0x0D55
    #define GL_DEPTH_BITS 0x0D56
    #define GL_STENCIL_BITS 0x0D57
    #define GL_ACCUM_RED_BITS 0x0D58
    #define GL_ACCUM_GREEN_BITS 0x0D59
    #define GL_ACCUM_BLUE_BITS 0x0D5A
    #define GL_ACCUM_ALPHA_BITS 0x0D5B
    #define GL_NAME_STACK_DEPTH 0x0D70
    #define GL_AUTO_NORMAL 0x0D80
    #define GL_MAP1_COLOR_4 0x0D90
    #define GL_MAP1_INDEX 0x0D91
    #define GL_MAP1_NORMAL 0x0D92
    #define GL_MAP1_TEXTURE_COORD_1 0x0D93
    #define GL_MAP1_TEXTURE_COORD_2 0x0D94
    #define GL_MAP1_TEXTURE_COORD_3 0x0D95
    #define GL_MAP1_TEXTURE_COORD_4 0x0D96
    #define GL_MAP1_VERTEX_3 0x0D97
    #define GL_MAP1_VERTEX_4 0x0D98
    #define GL_MAP2_COLOR_4 0x0DB0
    #define GL_MAP2_INDEX 0x0DB1
    #define GL_MAP2_NORMAL 0x0DB2
    #define GL_MAP2_TEXTURE_COORD_1 0x0DB3
    #define GL_MAP2_TEXTURE_COORD_2 0x0DB4
    #define GL_MAP2_TEXTURE_COORD_3 0x0DB5
    #define GL_MAP2_TEXTURE_COORD_4 0x0DB6
    #define GL_MAP2_VERTEX_3 0x0DB7
    #define GL_MAP2_VERTEX_4 0x0DB8
    #define GL_MAP1_GRID_DOMAIN 0x0DD0
    #define GL_MAP1_GRID_SEGMENTS 0x0DD1
    #define GL_MAP2_GRID_DOMAIN 0x0DD2
    #define GL_MAP2_GRID_SEGMENTS 0x0DD3
    #define GL_TEXTURE_1D 0x0DE0
    #define GL_TEXTURE_2D 0x0DE1
    #define GL_FEEDBACK_BUFFER_POINTER 0x0DF0
    #define GL_FEEDBACK_BUFFER_SIZE 0x0DF1
    #define GL_FEEDBACK_BUFFER_TYPE 0x0DF2
    #define GL_SELECTION_BUFFER_POINTER 0x0DF3
    #define GL_SELECTION_BUFFER_SIZE 0x0DF4
    #define GL_TEXTURE_WIDTH 0x1000
    #define GL_TRANSFORM_BIT 0x00001000
    #define GL_TEXTURE_HEIGHT 0x1001
    #define GL_TEXTURE_INTERNAL_FORMAT 0x1003
    #define GL_TEXTURE_BORDER_COLOR 0x1004
    #define GL_TEXTURE_BORDER 0x1005
    #define GL_DONT_CARE 0x1100
    #define GL_FASTEST 0x1101
    #define GL_NICEST 0x1102
    #define GL_AMBIENT 0x1200
    #define GL_DIFFUSE 0x1201
    #define GL_SPECULAR 0x1202
    #define GL_POSITION 0x1203
    #define GL_SPOT_DIRECTION 0x1204
    #define GL_SPOT_EXPONENT 0x1205
    #define GL_SPOT_CUTOFF 0x1206
    #define GL_CONSTANT_ATTENUATION 0x1207
    #define GL_LINEAR_ATTENUATION 0x1208
    #define GL_QUADRATIC_ATTENUATION 0x1209
    #define GL_COMPILE 0x1300
    #define GL_COMPILE_AND_EXECUTE 0x1301
    #define GL_BYTE 0x1400
    #define GL_UNSIGNED_BYTE 0x1401
    #define GL_SHORT 0x1402
    #define GL_UNSIGNED_SHORT 0x1403
    #define GL_INT 0x1404
    #define GL_UNSIGNED_INT 0x1405
    #define GL_FLOAT 0x1406
    #define GL_2_BYTES 0x1407
    #define GL_3_BYTES 0x1408
    #define GL_4_BYTES 0x1409
    #define GL_DOUBLE 0x140A
    #define GL_CLEAR 0x1500
    #define GL_AND 0x1501
    #define GL_AND_REVERSE 0x1502
    #define GL_COPY 0x1503
    #define GL_AND_INVERTED 0x1504
    #define GL_NOOP 0x1505
    #define GL_XOR 0x1506
    #define GL_OR 0x1507
    #define GL_NOR 0x1508
    #define GL_EQUIV 0x1509
    #define GL_INVERT 0x150A
    #define GL_OR_REVERSE 0x150B
    #define GL_COPY_INVERTED 0x150C
    #define GL_OR_INVERTED 0x150D
    #define GL_NAND 0x150E
    #define GL_SET 0x150F
    #define GL_EMISSION 0x1600
    #define GL_SHININESS 0x1601
    #define GL_AMBIENT_AND_DIFFUSE 0x1602
    #define GL_COLOR_INDEXES 0x1603
    #define GL_MODELVIEW 0x1700
    #define GL_PROJECTION 0x1701
    #define GL_TEXTURE 0x1702
    #define GL_COLOR 0x1800
    #define GL_DEPTH 0x1801
    #define GL_STENCIL 0x1802
    #define GL_COLOR_INDEX 0x1900
    #define GL_STENCIL_INDEX 0x1901
    #define GL_DEPTH_COMPONENT 0x1902
    #define GL_RED 0x1903
    #define GL_GREEN 0x1904
    #define GL_BLUE 0x1905
    #define GL_ALPHA 0x1906
    #define GL_RGB 0x1907
    #define GL_RGBA 0x1908
    #define GL_LUMINANCE 0x1909
    #define GL_LUMINANCE_ALPHA 0x190A
    #define GL_BITMAP 0x1A00
    #define GL_POINT 0x1B00
    #define GL_LINE 0x1B01
    #define GL_FILL 0x1B02
    #define GL_RENDER 0x1C00
    #define GL_FEEDBACK 0x1C01
    #define GL_SELECT 0x1C02
    #define GL_FLAT 0x1D00
    #define GL_SMOOTH 0x1D01
    #define GL_KEEP 0x1E00
    #define GL_REPLACE 0x1E01
    #define GL_INCR 0x1E02
    #define GL_DECR 0x1E03
    #define GL_VENDOR 0x1F00
    #define GL_RENDERER 0x1F01
    #define GL_VERSION 0x1F02
    #define GL_EXTENSIONS 0x1F03
    #define GL_S 0x2000
    #define GL_ENABLE_BIT 0x00002000
    #define GL_T 0x2001
    #define GL_R 0x2002
    #define GL_Q 0x2003
    #define GL_MODULATE 0x2100
    #define GL_DECAL 0x2101
    #define GL_TEXTURE_ENV_MODE 0x2200
    #define GL_TEXTURE_ENV_COLOR 0x2201
    #define GL_TEXTURE_ENV 0x2300
    #define GL_EYE_LINEAR 0x2400
    #define GL_OBJECT_LINEAR 0x2401
    #define GL_SPHERE_MAP 0x2402
    #define GL_TEXTURE_GEN_MODE 0x2500
    #define GL_OBJECT_PLANE 0x2501
    #define GL_EYE_PLANE 0x2502
    #define GL_NEAREST 0x2600
    #define GL_LINEAR 0x2601
    #define GL_NEAREST_MIPMAP_NEAREST 0x2700
    #define GL_LINEAR_MIPMAP_NEAREST 0x2701
    #define GL_NEAREST_MIPMAP_LINEAR 0x2702
    #define GL_LINEAR_MIPMAP_LINEAR 0x2703
    #define GL_TEXTURE_MAG_FILTER 0x2800
    #define GL_TEXTURE_MIN_FILTER 0x2801
    #define GL_TEXTURE_WRAP_S 0x2802
    #define GL_TEXTURE_WRAP_T 0x2803
    #define GL_CLAMP 0x2900
    #define GL_REPEAT 0x2901
    #define GL_POLYGON_OFFSET_UNITS 0x2A00
    #define GL_POLYGON_OFFSET_POINT 0x2A01
    #define GL_POLYGON_OFFSET_LINE 0x2A02
    #define GL_R3_G3_B2 0x2A10
    #define GL_V2F 0x2A20
    #define GL_V3F 0x2A21
    #define GL_C4UB_V2F 0x2A22
    #define GL_C4UB_V3F 0x2A23
    #define GL_C3F_V3F 0x2A24
    #define GL_N3F_V3F 0x2A25
    #define GL_C4F_N3F_V3F 0x2A26
    #define GL_T2F_V3F 0x2A27
    #define GL_T4F_V4F 0x2A28
    #define GL_T2F_C4UB_V3F 0x2A29
    #define GL_T2F_C3F_V3F 0x2A2A
    #define GL_T2F_N3F_V3F 0x2A2B
    #define GL_T2F_C4F_N3F_V3F 0x2A2C
    #define GL_T4F_C4F_N3F_V4F 0x2A2D
    #define GL_CLIP_PLANE0 0x3000
    #define GL_CLIP_PLANE1 0x3001
    #define GL_CLIP_PLANE2 0x3002
    #define GL_CLIP_PLANE3 0x3003
    #define GL_CLIP_PLANE4 0x3004
    #define GL_CLIP_PLANE5 0x3005
    #define GL_LIGHT0 0x4000
    #define GL_COLOR_BUFFER_BIT 0x00004000
    #define GL_LIGHT1 0x4001
    #define GL_LIGHT2 0x4002
    #define GL_LIGHT3 0x4003
    #define GL_LIGHT4 0x4004
    #define GL_LIGHT5 0x4005
    #define GL_LIGHT6 0x4006
    #define GL_LIGHT7 0x4007
    #define GL_HINT_BIT 0x00008000
    #define GL_POLYGON_OFFSET_FILL 0x8037
    #define GL_POLYGON_OFFSET_FACTOR 0x8038
    #define GL_ALPHA4 0x803B
    #define GL_ALPHA8 0x803C
    #define GL_ALPHA12 0x803D
    #define GL_ALPHA16 0x803E
    #define GL_LUMINANCE4 0x803F
    #define GL_LUMINANCE8 0x8040
    #define GL_LUMINANCE12 0x8041
    #define GL_LUMINANCE16 0x8042
    #define GL_LUMINANCE4_ALPHA4 0x8043
    #define GL_LUMINANCE6_ALPHA2 0x8044
    #define GL_LUMINANCE8_ALPHA8 0x8045
    #define GL_LUMINANCE12_ALPHA4 0x8046
    #define GL_LUMINANCE12_ALPHA12 0x8047
    #define GL_LUMINANCE16_ALPHA16 0x8048
    #define GL_INTENSITY 0x8049
    #define GL_INTENSITY4 0x804A
    #define GL_INTENSITY8 0x804B
    #define GL_INTENSITY12 0x804C
    #define GL_INTENSITY16 0x804D
    #define GL_RGB4 0x804F
    #define GL_RGB5 0x8050
    #define GL_RGB8 0x8051
    #define GL_RGB10 0x8052
    #define GL_RGB12 0x8053
    #define GL_RGB16 0x8054
    #define GL_RGBA2 0x8055
    #define GL_RGBA4 0x8056
    #define GL_RGB5_A1 0x8057
    #define GL_RGBA8 0x8058
    #define GL_RGB10_A2 0x8059
    #define GL_RGBA12 0x805A
    #define GL_RGBA16 0x805B
    #define GL_TEXTURE_RED_SIZE 0x805C
    #define GL_TEXTURE_GREEN_SIZE 0x805D
    #define GL_TEXTURE_BLUE_SIZE 0x805E
    #define GL_TEXTURE_ALPHA_SIZE 0x805F
    #define GL_TEXTURE_LUMINANCE_SIZE 0x8060
    #define GL_TEXTURE_INTENSITY_SIZE 0x8061
    #define GL_PROXY_TEXTURE_1D 0x8063
    #define GL_PROXY_TEXTURE_2D 0x8064
    #define GL_TEXTURE_PRIORITY 0x8066
    #define GL_TEXTURE_RESIDENT 0x8067
    #define GL_TEXTURE_BINDING_1D 0x8068
    #define GL_TEXTURE_BINDING_2D 0x8069
    #define GL_VERTEX_ARRAY 0x8074
    #define GL_NORMAL_ARRAY 0x8075
    #define GL_COLOR_ARRAY 0x8076
    #define GL_INDEX_ARRAY 0x8077
    #define GL_TEXTURE_COORD_ARRAY 0x8078
    #define GL_EDGE_FLAG_ARRAY 0x8079
    #define GL_VERTEX_ARRAY_SIZE 0x807A
    #define GL_VERTEX_ARRAY_TYPE 0x807B
    #define GL_VERTEX_ARRAY_STRIDE 0x807C
    #define GL_NORMAL_ARRAY_TYPE 0x807E
    #define GL_NORMAL_ARRAY_STRIDE 0x807F
    #define GL_COLOR_ARRAY_SIZE 0x8081
    #define GL_COLOR_ARRAY_TYPE 0x8082
    #define GL_COLOR_ARRAY_STRIDE 0x8083
    #define GL_INDEX_ARRAY_TYPE 0x8085
    #define GL_INDEX_ARRAY_STRIDE 0x8086
    #define GL_TEXTURE_COORD_ARRAY_SIZE 0x8088
    #define GL_TEXTURE_COORD_ARRAY_TYPE 0x8089
    #define GL_TEXTURE_COORD_ARRAY_STRIDE 0x808A
    #define GL_EDGE_FLAG_ARRAY_STRIDE 0x808C
    #define GL_VERTEX_ARRAY_POINTER 0x808E
    #define GL_NORMAL_ARRAY_POINTER 0x808F
    #define GL_COLOR_ARRAY_POINTER 0x8090
    #define GL_INDEX_ARRAY_POINTER 0x8091
    #define GL_TEXTURE_COORD_ARRAY_POINTER 0x8092
    #define GL_EDGE_FLAG_ARRAY_POINTER 0x8093
    #define GL_COLOR_INDEX1_EXT 0x80E2
    #define GL_COLOR_INDEX2_EXT 0x80E3
    #define GL_COLOR_INDEX4_EXT 0x80E4
    #define GL_COLOR_INDEX8_EXT 0x80E5
    #define GL_COLOR_INDEX12_EXT 0x80E6
    #define GL_COLOR_INDEX16_EXT 0x80E7
    #define GL_EVAL_BIT 0x00010000
    #define GL_LIST_BIT 0x00020000
    #define GL_TEXTURE_BIT 0x00040000
    #define GL_SCISSOR_BIT 0x00080000
    #define GL_ALL_ATTRIB_BITS 0x000fffff
    #define GL_CLIENT_ALL_ATTRIB_BITS 0xffffffff

    #if defined(GLE_HOOKING_ENABLED)
        // In this case we map these functions to internal versions instead of the standard versions.
        #define glAccum(...)                   GLEGetCurrentFunction(glAccum)(__VA_ARGS__)
        #define glAlphaFunc(...)               GLEGetCurrentFunction(glAlphaFunc)(__VA_ARGS__)
        #define glAreTexturesResident(...)     GLEGetCurrentFunction(glAreTexturesResident)(__VA_ARGS__)
        #define glArrayElement(...)            GLEGetCurrentFunction(glArrayElement)(__VA_ARGS__)
        #define glBegin(...)                   GLEGetCurrentFunction(glBegin)(__VA_ARGS__)
        #define glBindTexture(...)             GLEGetCurrentFunction(glBindTexture)(__VA_ARGS__)
        #define glBitmap(...)                  GLEGetCurrentFunction(glBitmap)(__VA_ARGS__)
        #define glBlendFunc(...)               GLEGetCurrentFunction(glBlendFunc)(__VA_ARGS__)
        #define glCallList(...)                GLEGetCurrentFunction(glCallList)(__VA_ARGS__)
        #define glCallLists(...)               GLEGetCurrentFunction(glCallLists)(__VA_ARGS__)
        #define glClear(...)                   GLEGetCurrentFunction(glClear)(__VA_ARGS__)
        #define glClearAccum(...)              GLEGetCurrentFunction(glClearAccum)(__VA_ARGS__)
        #define glClearColor(...)              GLEGetCurrentFunction(glClearColor)(__VA_ARGS__)
        #define glClearDepth(...)              GLEGetCurrentFunction(glClearDepth)(__VA_ARGS__)
        #define glClearIndex(...)              GLEGetCurrentFunction(glClearIndex)(__VA_ARGS__)
        #define glClearStencil(...)            GLEGetCurrentFunction(glClearStencil)(__VA_ARGS__)
        #define glClipPlane(...)               GLEGetCurrentFunction(glClipPlane)(__VA_ARGS__)
        #define glColor3b(...)                 GLEGetCurrentFunction(glColor3b)(__VA_ARGS__)
        #define glColor3bv(...)                GLEGetCurrentFunction(glColor3bv)(__VA_ARGS__)
        #define glColor3d(...)                 GLEGetCurrentFunction(glColor3d)(__VA_ARGS__)
        #define glColor3dv(...)                GLEGetCurrentFunction(glColor3dv)(__VA_ARGS__)
        #define glColor3f(...)                 GLEGetCurrentFunction(glColor3f)(__VA_ARGS__)
        #define glColor3fv(...)                GLEGetCurrentFunction(glColor3fv)(__VA_ARGS__)
        #define glColor3i(...)                 GLEGetCurrentFunction(glColor3i)(__VA_ARGS__)
        #define glColor3iv(...)                GLEGetCurrentFunction(glColor3iv)(__VA_ARGS__)
        #define glColor3s(...)                 GLEGetCurrentFunction(glColor3s)(__VA_ARGS__)
        #define glColor3sv(...)                GLEGetCurrentFunction(glColor3sv)(__VA_ARGS__)
        #define glColor3ub(...)                GLEGetCurrentFunction(glColor3ub)(__VA_ARGS__)
        #define glColor3ubv(...)               GLEGetCurrentFunction(glColor3ubv)(__VA_ARGS__)
        #define glColor3ui(...)                GLEGetCurrentFunction(glColor3ui)(__VA_ARGS__)
        #define glColor3uiv(...)               GLEGetCurrentFunction(glColor3uiv)(__VA_ARGS__)
        #define glColor3us(...)                GLEGetCurrentFunction(glColor3us)(__VA_ARGS__)
        #define glColor3usv(...)               GLEGetCurrentFunction(glColor3usv)(__VA_ARGS__)
        #define glColor4b(...)                 GLEGetCurrentFunction(glColor4b)(__VA_ARGS__)
        #define glColor4bv(...)                GLEGetCurrentFunction(glColor4bv)(__VA_ARGS__)
        #define glColor4d(...)                 GLEGetCurrentFunction(glColor4d)(__VA_ARGS__)
        #define glColor4dv(...)                GLEGetCurrentFunction(glColor4dv)(__VA_ARGS__)
        #define glColor4f(...)                 GLEGetCurrentFunction(glColor4f)(__VA_ARGS__)
        #define glColor4fv(...)                GLEGetCurrentFunction(glColor4fv)(__VA_ARGS__)
        #define glColor4i(...)                 GLEGetCurrentFunction(glColor4i)(__VA_ARGS__)
        #define glColor4iv(...)                GLEGetCurrentFunction(glColor4iv)(__VA_ARGS__)
        #define glColor4s(...)                 GLEGetCurrentFunction(glColor4s)(__VA_ARGS__)
        #define glColor4sv(...)                GLEGetCurrentFunction(glColor4sv)(__VA_ARGS__)
        #define glColor4ub(...)                GLEGetCurrentFunction(glColor4ub)(__VA_ARGS__)
        #define glColor4ubv(...)               GLEGetCurrentFunction(glColor4ubv)(__VA_ARGS__)
        #define glColor4ui(...)                GLEGetCurrentFunction(glColor4ui)(__VA_ARGS__)
        #define glColor4uiv(...)               GLEGetCurrentFunction(glColor4uiv)(__VA_ARGS__)
        #define glColor4us(...)                GLEGetCurrentFunction(glColor4us)(__VA_ARGS__)
        #define glColor4usv(...)               GLEGetCurrentFunction(glColor4usv)(__VA_ARGS__)
        #define glColorMask(...)               GLEGetCurrentFunction(glColorMask)(__VA_ARGS__)
        #define glColorMaterial(...)           GLEGetCurrentFunction(glColorMaterial)(__VA_ARGS__)
        #define glColorPointer(...)            GLEGetCurrentFunction(glColorPointer)(__VA_ARGS__)
        #define glCopyPixels(...)              GLEGetCurrentFunction(glCopyPixels)(__VA_ARGS__)
        #define glCopyTexImage1D(...)          GLEGetCurrentFunction(glCopyTexImage1D)(__VA_ARGS__)
        #define glCopyTexImage2D(...)          GLEGetCurrentFunction(glCopyTexImage2D)(__VA_ARGS__)
        #define glCopyTexSubImage1D(...)       GLEGetCurrentFunction(glCopyTexSubImage1D)(__VA_ARGS__)
        #define glCopyTexSubImage2D(...)       GLEGetCurrentFunction(glCopyTexSubImage2D)(__VA_ARGS__)
        #define glCullFace(...)                GLEGetCurrentFunction(glCullFace)(__VA_ARGS__)
        #define glDeleteLists(...)             GLEGetCurrentFunction(glDeleteLists)(__VA_ARGS__)
        #define glDeleteTextures(...)          GLEGetCurrentFunction(glDeleteTextures)(__VA_ARGS__)
        #define glDepthFunc(...)               GLEGetCurrentFunction(glDepthFunc)(__VA_ARGS__)
        #define glDepthMask(...)               GLEGetCurrentFunction(glDepthMask)(__VA_ARGS__)
        #define glDepthRange(...)              GLEGetCurrentFunction(glDepthRange)(__VA_ARGS__)
        #define glDisable(...)                 GLEGetCurrentFunction(glDisable)(__VA_ARGS__)
        #define glDisableClientState(...)      GLEGetCurrentFunction(glDisableClientState)(__VA_ARGS__)
        #define glDrawArrays(...)              GLEGetCurrentFunction(glDrawArrays)(__VA_ARGS__)
        #define glDrawBuffer(...)              GLEGetCurrentFunction(glDrawBuffer)(__VA_ARGS__)
        #define glDrawElements(...)            GLEGetCurrentFunction(glDrawElements)(__VA_ARGS__)
        #define glDrawPixels(...)              GLEGetCurrentFunction(glDrawPixels)(__VA_ARGS__)
        #define glEdgeFlag(...)                GLEGetCurrentFunction(glEdgeFlag)(__VA_ARGS__)
        #define glEdgeFlagPointer(...)         GLEGetCurrentFunction(glEdgeFlagPointer)(__VA_ARGS__)
        #define glEdgeFlagv(...)               GLEGetCurrentFunction(glEdgeFlagv)(__VA_ARGS__)
        #define glEnable(...)                  GLEGetCurrentFunction(glEnable)(__VA_ARGS__)
        #define glEnableClientState(...)       GLEGetCurrentFunction(glEnableClientState)(__VA_ARGS__)
        #define glEnd()                        GLEGetCurrentFunction(glEnd)()
        #define glEndList()                    GLEGetCurrentFunction(glEndList)(_)
        #define glEvalCoord1d(...)             GLEGetCurrentFunction(glEvalCoord1d)(__VA_ARGS__)
        #define glEvalCoord1dv(...)            GLEGetCurrentFunction(glEvalCoord1dv)(__VA_ARGS__)
        #define glEvalCoord1f(...)             GLEGetCurrentFunction(glEvalCoord1f)(__VA_ARGS__)
        #define glEvalCoord1fv(...)            GLEGetCurrentFunction(glEvalCoord1fv)(__VA_ARGS__)
        #define glEvalCoord2d(...)             GLEGetCurrentFunction(glEvalCoord2d)(__VA_ARGS__)
        #define glEvalCoord2dv(...)            GLEGetCurrentFunction(glEvalCoord2dv)(__VA_ARGS__)
        #define glEvalCoord2f(...)             GLEGetCurrentFunction(glEvalCoord2f)(__VA_ARGS__)
        #define glEvalCoord2fv(...)            GLEGetCurrentFunction(glEvalCoord2fv)(__VA_ARGS__)
        #define glEvalMesh1(...)               GLEGetCurrentFunction(glEvalMesh1)(__VA_ARGS__)
        #define glEvalMesh2(...)               GLEGetCurrentFunction(glEvalMesh2)(__VA_ARGS__)
        #define glEvalPoint1(...)              GLEGetCurrentFunction(glEvalPoint1)(__VA_ARGS__)
        #define glEvalPoint2(...)              GLEGetCurrentFunction(glEvalPoint2)(__VA_ARGS__)
        #define glFeedbackBuffer(...)          GLEGetCurrentFunction(glFeedbackBuffer)(__VA_ARGS__)
        #define glFinish()                     GLEGetCurrentFunction(glFinish)()
        #define glFlush()                      GLEGetCurrentFunction(glFlush)()
        #define glFogf(...)                    GLEGetCurrentFunction(glFogf)(__VA_ARGS__)
        #define glFogfv(...)                   GLEGetCurrentFunction(glFogfv)(__VA_ARGS__)
        #define glFogi(...)                    GLEGetCurrentFunction(glFogi)(__VA_ARGS__)
        #define glFogiv(...)                   GLEGetCurrentFunction(glFogiv)(__VA_ARGS__)
        #define glFrontFace(...)               GLEGetCurrentFunction(glFrontFace)(__VA_ARGS__)
        #define glFrustum(...)                 GLEGetCurrentFunction(glFrustum)(__VA_ARGS__)
        #define glGenLists(...)                GLEGetCurrentFunction(glGenLists)(__VA_ARGS__)
        #define glGenTextures(...)             GLEGetCurrentFunction(glGenTextures)(__VA_ARGS__)
        #define glGetBooleanv(...)             GLEGetCurrentFunction(glGetBooleanv)(__VA_ARGS__)
        #define glGetClipPlane(...)            GLEGetCurrentFunction(glGetClipPlane)(__VA_ARGS__)
        #define glGetDoublev(...)              GLEGetCurrentFunction(glGetDoublev)(__VA_ARGS__)
        #define glGetError()                   GLEGetCurrentFunction(glGetError)()
        #define glGetFloatv(...)               GLEGetCurrentFunction(glGetFloatv)(__VA_ARGS__)
        #define glGetIntegerv(...)             GLEGetCurrentFunction(glGetIntegerv)(__VA_ARGS__)
        #define glGetLightfv(...)              GLEGetCurrentFunction(glGetLightfv)(__VA_ARGS__)
        #define glGetLightiv(...)              GLEGetCurrentFunction(glGetLightiv)(__VA_ARGS__)
        #define glGetMapdv(...)                GLEGetCurrentFunction(glGetMapdv)(__VA_ARGS__)
        #define glGetMapfv(...)                GLEGetCurrentFunction(glGetMapfv)(__VA_ARGS__)
        #define glGetMapiv(...)                GLEGetCurrentFunction(glGetMapiv)(__VA_ARGS__)
        #define glGetMaterialfv(...)           GLEGetCurrentFunction(glGetMaterialfv)(__VA_ARGS__)
        #define glGetMaterialiv(...)           GLEGetCurrentFunction(glGetMaterialiv)(__VA_ARGS__)
        #define glGetPixelMapfv(...)           GLEGetCurrentFunction(glGetPixelMapfv)(__VA_ARGS__)
        #define glGetPixelMapuiv(...)          GLEGetCurrentFunction(glGetPixelMapuiv)(__VA_ARGS__)
        #define glGetPixelMapusv(...)          GLEGetCurrentFunction(glGetPixelMapusv)(__VA_ARGS__)
        #define glGetPointerv(...)             GLEGetCurrentFunction(glGetPointerv)(__VA_ARGS__)
        #define glGetPolygonStipple(...)       GLEGetCurrentFunction(glGetPolygonStipple)(__VA_ARGS__)
        #define glGetString(...)               GLEGetCurrentFunction(glGetString)(__VA_ARGS__)
        #define glGetTexEnvfv(...)             GLEGetCurrentFunction(glGetTexEnvfv)(__VA_ARGS__)
        #define glGetTexEnviv(...)             GLEGetCurrentFunction(glGetTexEnviv)(__VA_ARGS__)
        #define glGetTexGendv(...)             GLEGetCurrentFunction(glGetTexGendv)(__VA_ARGS__)
        #define glGetTexGenfv(...)             GLEGetCurrentFunction(glGetTexGenfv)(__VA_ARGS__)
        #define glGetTexGeniv(...)             GLEGetCurrentFunction(glGetTexGeniv)(__VA_ARGS__)
        #define glGetTexImage(...)             GLEGetCurrentFunction(glGetTexImage)(__VA_ARGS__)
        #define glGetTexLevelParameterfv(...)  GLEGetCurrentFunction(glGetTexLevelParameterfv)(__VA_ARGS__)
        #define glGetTexLevelParameteriv(...)  GLEGetCurrentFunction(glGetTexLevelParameteriv)(__VA_ARGS__)
        #define glGetTexParameterfv(...)       GLEGetCurrentFunction(glGetTexParameterfv)(__VA_ARGS__)
        #define glGetTexParameteriv(...)       GLEGetCurrentFunction(glGetTexParameteriv)(__VA_ARGS__)
        #define glHint(...)                    GLEGetCurrentFunction(glHint)(__VA_ARGS__)
        #define glIndexMask(...)               GLEGetCurrentFunction(glIndexMask)(__VA_ARGS__)
        #define glIndexPointer(...)            GLEGetCurrentFunction(glIndexPointer)(__VA_ARGS__)
        #define glIndexd(...)                  GLEGetCurrentFunction(glIndexd)(__VA_ARGS__)
        #define glIndexdv(...)                 GLEGetCurrentFunction(glIndexdv)(__VA_ARGS__)
        #define glIndexf(...)                  GLEGetCurrentFunction(glIndexf)(__VA_ARGS__)
        #define glIndexfv(...)                 GLEGetCurrentFunction(glIndexfv)(__VA_ARGS__)
        #define glIndexi(...)                  GLEGetCurrentFunction(glIndexi)(__VA_ARGS__)
        #define glIndexiv(...)                 GLEGetCurrentFunction(glIndexiv)(__VA_ARGS__)
        #define glIndexs(...)                  GLEGetCurrentFunction(glIndexs)(__VA_ARGS__)
        #define glIndexsv(...)                 GLEGetCurrentFunction(glIndexsv)(__VA_ARGS__)
        #define glIndexub(...)                 GLEGetCurrentFunction(glIndexub)(__VA_ARGS__)
        #define glIndexubv(...)                GLEGetCurrentFunction(glIndexubv)(__VA_ARGS__)
        #define glInitNames()                  GLEGetCurrentFunction(glInitNames)()
        #define glInterleavedArrays(...)       GLEGetCurrentFunction(glInterleavedArrays)(__VA_ARGS__)
        #define glIsEnabled(...)               GLEGetCurrentFunction(glIsEnabled)(__VA_ARGS__)
        #define glIsList(...)                  GLEGetCurrentFunction(glIsList)(__VA_ARGS__)
        #define glIsTexture(...)               GLEGetCurrentFunction(glIsTexture)(__VA_ARGS__)
        #define glLightModelf(...)             GLEGetCurrentFunction(glLightModelf)(__VA_ARGS__)
        #define glLightModelfv(...)            GLEGetCurrentFunction(glLightModelfv)(__VA_ARGS__)
        #define glLightModeli(...)             GLEGetCurrentFunction(glLightModeli)(__VA_ARGS__)
        #define glLightModeliv(...)            GLEGetCurrentFunction(glLightModeliv)(__VA_ARGS__)
        #define glLightf(...)                  GLEGetCurrentFunction(glLightf)(__VA_ARGS__)
        #define glLightfv(...)                 GLEGetCurrentFunction(glLightfv)(__VA_ARGS__)
        #define glLighti(...)                  GLEGetCurrentFunction(glLighti)(__VA_ARGS__)
        #define glLightiv(...)                 GLEGetCurrentFunction(glLightiv)(__VA_ARGS__)
        #define glLineStipple(...)             GLEGetCurrentFunction(glLineStipple)(__VA_ARGS__)
        #define glLineWidth(...)               GLEGetCurrentFunction(glLineWidth)(__VA_ARGS__)
        #define glListBase(...)                GLEGetCurrentFunction(glListBase)(__VA_ARGS__)
        #define glLoadIdentity()               GLEGetCurrentFunction(glLoadIdentity)()
        #define glLoadMatrixd(...)             GLEGetCurrentFunction(glLoadMatrixd)(__VA_ARGS__)
        #define glLoadMatrixf(...)             GLEGetCurrentFunction(glLoadMatrixf)(__VA_ARGS__)
        #define glLoadName(...)                GLEGetCurrentFunction(glLoadName)(__VA_ARGS__)
        #define glLogicOp(...)                 GLEGetCurrentFunction(glLogicOp)(__VA_ARGS__)
        #define glMap1d(...)                   GLEGetCurrentFunction(glMap1d)(__VA_ARGS__)
        #define glMap1f(...)                   GLEGetCurrentFunction(glMap1f)(__VA_ARGS__)
        #define glMap2d(...)                   GLEGetCurrentFunction(glMap2d)(__VA_ARGS__)
        #define glMap2f(...)                   GLEGetCurrentFunction(glMap2f)(__VA_ARGS__)
        #define glMapGrid1d(...)               GLEGetCurrentFunction(glMapGrid1d)(__VA_ARGS__)
        #define glMapGrid1f(...)               GLEGetCurrentFunction(glMapGrid1f)(__VA_ARGS__)
        #define glMapGrid2d(...)               GLEGetCurrentFunction(glMapGrid2d)(__VA_ARGS__)
        #define glMapGrid2f(...)               GLEGetCurrentFunction(glMapGrid2f)(__VA_ARGS__)
        #define glMaterialf(...)               GLEGetCurrentFunction(glMaterialf)(__VA_ARGS__)
        #define glMaterialfv(...)              GLEGetCurrentFunction(glMaterialfv)(__VA_ARGS__)
        #define glMateriali(...)               GLEGetCurrentFunction(glMateriali)(__VA_ARGS__)
        #define glMaterialiv(...)              GLEGetCurrentFunction(glMaterialiv)(__VA_ARGS__)
        #define glMatrixMode(...)              GLEGetCurrentFunction(glMatrixMode)(__VA_ARGS__)
        #define glMultMatrixd(...)             GLEGetCurrentFunction(glMultMatrixd)(__VA_ARGS__)
        #define glMultMatrixf(...)             GLEGetCurrentFunction(glMultMatrixf)(__VA_ARGS__)
        #define glNewList(...)                 GLEGetCurrentFunction(glNewList)(__VA_ARGS__)
        #define glNormal3b(...)                GLEGetCurrentFunction(glNormal3b)(__VA_ARGS__)
        #define glNormal3bv(...)               GLEGetCurrentFunction(glNormal3bv)(__VA_ARGS__)
        #define glNormal3d(...)                GLEGetCurrentFunction(glNormal3d)(__VA_ARGS__)
        #define glNormal3dv(...)               GLEGetCurrentFunction(glNormal3dv)(__VA_ARGS__)
        #define glNormal3f(...)                GLEGetCurrentFunction(glNormal3f)(__VA_ARGS__)
        #define glNormal3fv(...)               GLEGetCurrentFunction(glNormal3fv)(__VA_ARGS__)
        #define glNormal3i(...)                GLEGetCurrentFunction(glNormal3i)(__VA_ARGS__)
        #define glNormal3iv(...)               GLEGetCurrentFunction(glNormal3iv)(__VA_ARGS__)
        #define glNormal3s(...)                GLEGetCurrentFunction(glNormal3s)(__VA_ARGS__)
        #define glNormal3sv(...)               GLEGetCurrentFunction(glNormal3sv)(__VA_ARGS__)
        #define glNormalPointer(...)           GLEGetCurrentFunction(glNormalPointer)(__VA_ARGS__)
        #define glOrtho(...)                   GLEGetCurrentFunction(glOrtho)(__VA_ARGS__)
        #define glPassThrough(...)             GLEGetCurrentFunction(glPassThrough)(__VA_ARGS__)
        #define glPixelMapfv(...)              GLEGetCurrentFunction(glPixelMapfv)(__VA_ARGS__)
        #define glPixelMapuiv(...)             GLEGetCurrentFunction(glPixelMapuiv)(__VA_ARGS__)
        #define glPixelMapusv(...)             GLEGetCurrentFunction(glPixelMapusv)(__VA_ARGS__)
        #define glPixelStoref(...)             GLEGetCurrentFunction(glPixelStoref)(__VA_ARGS__)
        #define glPixelStorei(...)             GLEGetCurrentFunction(glPixelStorei)(__VA_ARGS__)
        #define glPixelTransferf(...)          GLEGetCurrentFunction(glPixelTransferf)(__VA_ARGS__)
        #define glPixelTransferi(...)          GLEGetCurrentFunction(glPixelTransferi)(__VA_ARGS__)
        #define glPixelZoom(...)               GLEGetCurrentFunction(glPixelZoom)(__VA_ARGS__)
        #define glPointSize(...)               GLEGetCurrentFunction(glPointSize)(__VA_ARGS__)
        #define glPolygonMode(...)             GLEGetCurrentFunction(glPolygonMode)(__VA_ARGS__)
        #define glPolygonOffset(...)           GLEGetCurrentFunction(glPolygonOffset)(__VA_ARGS__)
        #define glPolygonStipple(...)          GLEGetCurrentFunction(glPolygonStipple)(__VA_ARGS__)
        #define glPopAttrib()                  GLEGetCurrentFunction(glPopAttrib)()
        #define glPopClientAttrib()            GLEGetCurrentFunction(glPopClientAttrib)()
        #define glPopMatrix()                  GLEGetCurrentFunction(glPopMatrix)()
        #define glPopName()                    GLEGetCurrentFunction(glPopName)()
        #define glPrioritizeTextures(...)      GLEGetCurrentFunction(glPrioritizeTextures)(__VA_ARGS__)
        #define glPushAttrib(...)              GLEGetCurrentFunction(glPushAttrib)(__VA_ARGS__)
        #define glPushClientAttrib(...)        GLEGetCurrentFunction(glPushClientAttrib)(__VA_ARGS__)
        #define glPushMatrix()                 GLEGetCurrentFunction(glPushMatrix)()
        #define glPushName(...)                GLEGetCurrentFunction(glPushName)(__VA_ARGS__)
        #define glRasterPos2d(...)             GLEGetCurrentFunction(glRasterPos2d)(__VA_ARGS__)
        #define glRasterPos2dv(...)            GLEGetCurrentFunction(glRasterPos2dv)(__VA_ARGS__)
        #define glRasterPos2f(...)             GLEGetCurrentFunction(glRasterPos2f)(__VA_ARGS__)
        #define glRasterPos2fv(...)            GLEGetCurrentFunction(glRasterPos2fv)(__VA_ARGS__)
        #define glRasterPos2i(...)             GLEGetCurrentFunction(glRasterPos2i)(__VA_ARGS__)
        #define glRasterPos2iv(...)            GLEGetCurrentFunction(glRasterPos2iv)(__VA_ARGS__)
        #define glRasterPos2s(...)             GLEGetCurrentFunction(glRasterPos2s)(__VA_ARGS__)
        #define glRasterPos2sv(...)            GLEGetCurrentFunction(glRasterPos2sv)(__VA_ARGS__)
        #define glRasterPos3d(...)             GLEGetCurrentFunction(glRasterPos3d)(__VA_ARGS__)
        #define glRasterPos3dv(...)            GLEGetCurrentFunction(glRasterPos3dv)(__VA_ARGS__)
        #define glRasterPos3f(...)             GLEGetCurrentFunction(glRasterPos3f)(__VA_ARGS__)
        #define glRasterPos3fv(...)            GLEGetCurrentFunction(glRasterPos3fv)(__VA_ARGS__)
        #define glRasterPos3i(...)             GLEGetCurrentFunction(glRasterPos3i)(__VA_ARGS__)
        #define glRasterPos3iv(...)            GLEGetCurrentFunction(glRasterPos3iv)(__VA_ARGS__)
        #define glRasterPos3s(...)             GLEGetCurrentFunction(glRasterPos3s)(__VA_ARGS__)
        #define glRasterPos3sv(...)            GLEGetCurrentFunction(glRasterPos3sv)(__VA_ARGS__)
        #define glRasterPos4d(...)             GLEGetCurrentFunction(glRasterPos4d)(__VA_ARGS__)
        #define glRasterPos4dv(...)            GLEGetCurrentFunction(glRasterPos4dv)(__VA_ARGS__)
        #define glRasterPos4f(...)             GLEGetCurrentFunction(glRasterPos4f)(__VA_ARGS__)
        #define glRasterPos4fv(...)            GLEGetCurrentFunction(glRasterPos4fv)(__VA_ARGS__)
        #define glRasterPos4i(...)             GLEGetCurrentFunction(glRasterPos4i)(__VA_ARGS__)
        #define glRasterPos4iv(...)            GLEGetCurrentFunction(glRasterPos4iv)(__VA_ARGS__)
        #define glRasterPos4s(...)             GLEGetCurrentFunction(glRasterPos4s)(__VA_ARGS__)
        #define glRasterPos4sv(...)            GLEGetCurrentFunction(glRasterPos4sv)(__VA_ARGS__)
        #define glReadBuffer(...)              GLEGetCurrentFunction(glReadBuffer)(__VA_ARGS__)
        #define glReadPixels(...)              GLEGetCurrentFunction(glReadPixels)(__VA_ARGS__)
        #define glRectd(...)                   GLEGetCurrentFunction(glRectd)(__VA_ARGS__)
        #define glRectdv(...)                  GLEGetCurrentFunction(glRectdv)(__VA_ARGS__)
        #define glRectf(...)                   GLEGetCurrentFunction(glRectf)(__VA_ARGS__)
        #define glRectfv(...)                  GLEGetCurrentFunction(glRectfv)(__VA_ARGS__)
        #define glRecti(...)                   GLEGetCurrentFunction(glRecti)(__VA_ARGS__)
        #define glRectiv(...)                  GLEGetCurrentFunction(glRectiv)(__VA_ARGS__)
        #define glRects(...)                   GLEGetCurrentFunction(glRects)(__VA_ARGS__)
        #define glRectsv(...)                  GLEGetCurrentFunction(glRectsv)(__VA_ARGS__)
        #define glRenderMode(...)              GLEGetCurrentFunction(glRenderMode)(__VA_ARGS__)
        #define glRotated(...)                 GLEGetCurrentFunction(glRotated)(__VA_ARGS__)
        #define glRotatef(...)                 GLEGetCurrentFunction(glRotatef)(__VA_ARGS__)
        #define glScaled(...)                  GLEGetCurrentFunction(glScaled)(__VA_ARGS__)
        #define glScalef(...)                  GLEGetCurrentFunction(glScalef)(__VA_ARGS__)
        #define glScissor(...)                 GLEGetCurrentFunction(glScissor)(__VA_ARGS__)
        #define glSelectBuffer(...)            GLEGetCurrentFunction(glSelectBuffer)(__VA_ARGS__)
        #define glShadeModel(...)              GLEGetCurrentFunction(glShadeModel)(__VA_ARGS__)
        #define glStencilFunc(...)             GLEGetCurrentFunction(glStencilFunc)(__VA_ARGS__)
        #define glStencilMask(...)             GLEGetCurrentFunction(glStencilMask)(__VA_ARGS__)
        #define glStencilOp(...)               GLEGetCurrentFunction(glStencilOp)(__VA_ARGS__)
        #define glTexCoord1d(...)              GLEGetCurrentFunction(glTexCoord1d)(__VA_ARGS__)
        #define glTexCoord1dv(...)             GLEGetCurrentFunction(glTexCoord1dv)(__VA_ARGS__)
        #define glTexCoord1f(...)              GLEGetCurrentFunction(glTexCoord1f)(__VA_ARGS__)
        #define glTexCoord1fv(...)             GLEGetCurrentFunction(glTexCoord1fv)(__VA_ARGS__)
        #define glTexCoord1i(...)              GLEGetCurrentFunction(glTexCoord1i)(__VA_ARGS__)
        #define glTexCoord1iv(...)             GLEGetCurrentFunction(glTexCoord1iv)(__VA_ARGS__)
        #define glTexCoord1s(...)              GLEGetCurrentFunction(glTexCoord1s)(__VA_ARGS__)
        #define glTexCoord1sv(...)             GLEGetCurrentFunction(glTexCoord1sv)(__VA_ARGS__)
        #define glTexCoord2d(...)              GLEGetCurrentFunction(glTexCoord2d)(__VA_ARGS__)
        #define glTexCoord2dv(...)             GLEGetCurrentFunction(glTexCoord2dv)(__VA_ARGS__)
        #define glTexCoord2f(...)              GLEGetCurrentFunction(glTexCoord2f)(__VA_ARGS__)
        #define glTexCoord2fv(...)             GLEGetCurrentFunction(glTexCoord2fv)(__VA_ARGS__)
        #define glTexCoord2i(...)              GLEGetCurrentFunction(glTexCoord2i)(__VA_ARGS__)
        #define glTexCoord2iv(...)             GLEGetCurrentFunction(glTexCoord2iv)(__VA_ARGS__)
        #define glTexCoord2s(...)              GLEGetCurrentFunction(glTexCoord2s)(__VA_ARGS__)
        #define glTexCoord2sv(...)             GLEGetCurrentFunction(glTexCoord2sv)(__VA_ARGS__)
        #define glTexCoord3d(...)              GLEGetCurrentFunction(glTexCoord3d)(__VA_ARGS__)
        #define glTexCoord3dv(...)             GLEGetCurrentFunction(glTexCoord3dv)(__VA_ARGS__)
        #define glTexCoord3f(...)              GLEGetCurrentFunction(glTexCoord3f)(__VA_ARGS__)
        #define glTexCoord3fv(...)             GLEGetCurrentFunction(glTexCoord3fv)(__VA_ARGS__)
        #define glTexCoord3i(...)              GLEGetCurrentFunction(glTexCoord3i)(__VA_ARGS__)
        #define glTexCoord3iv(...)             GLEGetCurrentFunction(glTexCoord3iv)(__VA_ARGS__)
        #define glTexCoord3s(...)              GLEGetCurrentFunction(glTexCoord3s)(__VA_ARGS__)
        #define glTexCoord3sv(...)             GLEGetCurrentFunction(glTexCoord3sv)(__VA_ARGS__)
        #define glTexCoord4d(...)              GLEGetCurrentFunction(glTexCoord4d)(__VA_ARGS__)
        #define glTexCoord4dv(...)             GLEGetCurrentFunction(glTexCoord4dv)(__VA_ARGS__)
        #define glTexCoord4f(...)              GLEGetCurrentFunction(glTexCoord4f)(__VA_ARGS__)
        #define glTexCoord4fv(...)             GLEGetCurrentFunction(glTexCoord4fv)(__VA_ARGS__)
        #define glTexCoord4i(...)              GLEGetCurrentFunction(glTexCoord4i)(__VA_ARGS__)
        #define glTexCoord4iv(...)             GLEGetCurrentFunction(glTexCoord4iv)(__VA_ARGS__)
        #define glTexCoord4s(...)              GLEGetCurrentFunction(glTexCoord4s)(__VA_ARGS__)
        #define glTexCoord4sv(...)             GLEGetCurrentFunction(glTexCoord4sv)(__VA_ARGS__)
        #define glTexCoordPointer(...)         GLEGetCurrentFunction(glTexCoordPointer)(__VA_ARGS__)
        #define glTexEnvf(...)                 GLEGetCurrentFunction(glTexEnvf)(__VA_ARGS__)
        #define glTexEnvfv(...)                GLEGetCurrentFunction(glTexEnvfv)(__VA_ARGS__)
        #define glTexEnvi(...)                 GLEGetCurrentFunction(glTexEnvi)(__VA_ARGS__)
        #define glTexEnviv(...)                GLEGetCurrentFunction(glTexEnviv)(__VA_ARGS__)
        #define glTexGend(...)                 GLEGetCurrentFunction(glTexGend)(__VA_ARGS__)
        #define glTexGendv(...)                GLEGetCurrentFunction(glTexGendv)(__VA_ARGS__)
        #define glTexGenf(...)                 GLEGetCurrentFunction(glTexGenf)(__VA_ARGS__)
        #define glTexGenfv(...)                GLEGetCurrentFunction(glTexGenfv)(__VA_ARGS__)
        #define glTexGeni(...)                 GLEGetCurrentFunction(glTexGeni)(__VA_ARGS__)
        #define glTexGeniv(...)                GLEGetCurrentFunction(glTexGeniv)(__VA_ARGS__)
        #define glTexImage1D(...)              GLEGetCurrentFunction(glTexImage1D)(__VA_ARGS__)
        #define glTexImage2D(...)              GLEGetCurrentFunction(glTexImage2D)(__VA_ARGS__)
        #define glTexParameterf(...)           GLEGetCurrentFunction(glTexParameterf)(__VA_ARGS__)
        #define glTexParameterfv(...)          GLEGetCurrentFunction(glTexParameterfv)(__VA_ARGS__)
        #define glTexParameteri(...)           GLEGetCurrentFunction(glTexParameteri)(__VA_ARGS__)
        #define glTexParameteriv(...)          GLEGetCurrentFunction(glTexParameteriv)(__VA_ARGS__)
        #define glTexSubImage1D(...)           GLEGetCurrentFunction(glTexSubImage1D)(__VA_ARGS__)
        #define glTexSubImage2D(...)           GLEGetCurrentFunction(glTexSubImage2D)(__VA_ARGS__)
        #define glTranslated(...)              GLEGetCurrentFunction(glTranslated)(__VA_ARGS__)
        #define glTranslatef(...)              GLEGetCurrentFunction(glTranslatef)(__VA_ARGS__)
        #define glVertex2d(...)                GLEGetCurrentFunction(glVertex2d)(__VA_ARGS__)
        #define glVertex2dv(...)               GLEGetCurrentFunction(glVertex2dv)(__VA_ARGS__)
        #define glVertex2f(...)                GLEGetCurrentFunction(glVertex2f)(__VA_ARGS__)
        #define glVertex2fv(...)               GLEGetCurrentFunction(glVertex2fv)(__VA_ARGS__)
        #define glVertex2i(...)                GLEGetCurrentFunction(glVertex2i)(__VA_ARGS__)
        #define glVertex2iv(...)               GLEGetCurrentFunction(glVertex2iv)(__VA_ARGS__)
        #define glVertex2s(...)                GLEGetCurrentFunction(glVertex2s)(__VA_ARGS__)
        #define glVertex2sv(...)               GLEGetCurrentFunction(glVertex2sv)(__VA_ARGS__)
        #define glVertex3d(...)                GLEGetCurrentFunction(glVertex3d)(__VA_ARGS__)
        #define glVertex3dv(...)               GLEGetCurrentFunction(glVertex3dv)(__VA_ARGS__)
        #define glVertex3f(...)                GLEGetCurrentFunction(glVertex3f)(__VA_ARGS__)
        #define glVertex3fv(...)               GLEGetCurrentFunction(glVertex3fv)(__VA_ARGS__)
        #define glVertex3i(...)                GLEGetCurrentFunction(glVertex3i)(__VA_ARGS__)
        #define glVertex3iv(...)               GLEGetCurrentFunction(glVertex3iv)(__VA_ARGS__)
        #define glVertex3s(...)                GLEGetCurrentFunction(glVertex3s)(__VA_ARGS__)
        #define glVertex3sv(...)               GLEGetCurrentFunction(glVertex3sv)(__VA_ARGS__)
        #define glVertex4d(...)                GLEGetCurrentFunction(glVertex4d)(__VA_ARGS__)
        #define glVertex4dv(...)               GLEGetCurrentFunction(glVertex4dv)(__VA_ARGS__)
        #define glVertex4f(...)                GLEGetCurrentFunction(glVertex4f)(__VA_ARGS__)
        #define glVertex4fv(...)               GLEGetCurrentFunction(glVertex4fv)(__VA_ARGS__)
        #define glVertex4i(...)                GLEGetCurrentFunction(glVertex4i)(__VA_ARGS__)
        #define glVertex4iv(...)               GLEGetCurrentFunction(glVertex4iv)(__VA_ARGS__)
        #define glVertex4s(...)                GLEGetCurrentFunction(glVertex4s)(__VA_ARGS__)
        #define glVertex4sv(...)               GLEGetCurrentFunction(glVertex4sv)(__VA_ARGS__)
        #define glVertexPointer(...)           GLEGetCurrentFunction(glVertexPointer)(__VA_ARGS__)
        #define glViewport(...)                GLEGetCurrentFunction(glViewport)(__VA_ARGS__)
    #else
        // There is no need to typedef OpenGL 1.1 function types because they are present in all
        // OpenGL implementations and don't need to be treated dynamically like extensions.
        GLAPI void            GLAPIENTRY glAccum (GLenum op, GLfloat value);
        GLAPI void            GLAPIENTRY glAlphaFunc (GLenum func, GLclampf ref);
        GLAPI GLboolean       GLAPIENTRY glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
        GLAPI void            GLAPIENTRY glArrayElement (GLint i);
        GLAPI void            GLAPIENTRY glBegin (GLenum mode);
        GLAPI void            GLAPIENTRY glBindTexture (GLenum target, GLuint texture);
        GLAPI void            GLAPIENTRY glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
        GLAPI void            GLAPIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
        GLAPI void            GLAPIENTRY glCallList (GLuint list);
        GLAPI void            GLAPIENTRY glCallLists (GLsizei n, GLenum type, const void *lists);
        GLAPI void            GLAPIENTRY glClear (GLbitfield mask);
        GLAPI void            GLAPIENTRY glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        GLAPI void            GLAPIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
        GLAPI void            GLAPIENTRY glClearDepth (GLclampd depth);
        GLAPI void            GLAPIENTRY glClearIndex (GLfloat c);
        GLAPI void            GLAPIENTRY glClearStencil (GLint s);
        GLAPI void            GLAPIENTRY glClipPlane (GLenum plane, const GLdouble *equation);
        GLAPI void            GLAPIENTRY glColor3b (GLbyte red, GLbyte green, GLbyte blue);
        GLAPI void            GLAPIENTRY glColor3bv (const GLbyte *v);
        GLAPI void            GLAPIENTRY glColor3d (GLdouble red, GLdouble green, GLdouble blue);
        GLAPI void            GLAPIENTRY glColor3dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glColor3f (GLfloat red, GLfloat green, GLfloat blue);
        GLAPI void            GLAPIENTRY glColor3fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glColor3i (GLint red, GLint green, GLint blue);
        GLAPI void            GLAPIENTRY glColor3iv (const GLint *v);
        GLAPI void            GLAPIENTRY glColor3s (GLshort red, GLshort green, GLshort blue);
        GLAPI void            GLAPIENTRY glColor3sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glColor3ub (GLubyte red, GLubyte green, GLubyte blue);
        GLAPI void            GLAPIENTRY glColor3ubv (const GLubyte *v);
        GLAPI void            GLAPIENTRY glColor3ui (GLuint red, GLuint green, GLuint blue);
        GLAPI void            GLAPIENTRY glColor3uiv (const GLuint *v);
        GLAPI void            GLAPIENTRY glColor3us (GLushort red, GLushort green, GLushort blue);
        GLAPI void            GLAPIENTRY glColor3usv (const GLushort *v);
        GLAPI void            GLAPIENTRY glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
        GLAPI void            GLAPIENTRY glColor4bv (const GLbyte *v);
        GLAPI void            GLAPIENTRY glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
        GLAPI void            GLAPIENTRY glColor4dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        GLAPI void            GLAPIENTRY glColor4fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glColor4i (GLint red, GLint green, GLint blue, GLint alpha);
        GLAPI void            GLAPIENTRY glColor4iv (const GLint *v);
        GLAPI void            GLAPIENTRY glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha);
        GLAPI void            GLAPIENTRY glColor4sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
        GLAPI void            GLAPIENTRY glColor4ubv (const GLubyte *v);
        GLAPI void            GLAPIENTRY glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha);
        GLAPI void            GLAPIENTRY glColor4uiv (const GLuint *v);
        GLAPI void            GLAPIENTRY glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha);
        GLAPI void            GLAPIENTRY glColor4usv (const GLushort *v);
        GLAPI void            GLAPIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
        GLAPI void            GLAPIENTRY glColorMaterial (GLenum face, GLenum mode);
        GLAPI void            GLAPIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const void *pointer);
        GLAPI void            GLAPIENTRY glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
        GLAPI void            GLAPIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
        GLAPI void            GLAPIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
        GLAPI void            GLAPIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
        GLAPI void            GLAPIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
        GLAPI void            GLAPIENTRY glCullFace (GLenum mode);
        GLAPI void            GLAPIENTRY glDeleteLists (GLuint list, GLsizei range);
        GLAPI void            GLAPIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
        GLAPI void            GLAPIENTRY glDepthFunc (GLenum func);
        GLAPI void            GLAPIENTRY glDepthMask (GLboolean flag);
        GLAPI void            GLAPIENTRY glDepthRange (GLclampd zNear, GLclampd zFar);
        GLAPI void            GLAPIENTRY glDisable (GLenum cap);
        GLAPI void            GLAPIENTRY glDisableClientState (GLenum array);
        GLAPI void            GLAPIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
        GLAPI void            GLAPIENTRY glDrawBuffer (GLenum mode);
        GLAPI void            GLAPIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);
        GLAPI void            GLAPIENTRY glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
        GLAPI void            GLAPIENTRY glEdgeFlag (GLboolean flag);
        GLAPI void            GLAPIENTRY glEdgeFlagPointer (GLsizei stride, const void *pointer);
        GLAPI void            GLAPIENTRY glEdgeFlagv (const GLboolean *flag);
        GLAPI void            GLAPIENTRY glEnable (GLenum cap);
        GLAPI void            GLAPIENTRY glEnableClientState (GLenum array);
        GLAPI void            GLAPIENTRY glEnd (void);
        GLAPI void            GLAPIENTRY glEndList (void);
        GLAPI void            GLAPIENTRY glEvalCoord1d (GLdouble u);
        GLAPI void            GLAPIENTRY glEvalCoord1dv (const GLdouble *u);
        GLAPI void            GLAPIENTRY glEvalCoord1f (GLfloat u);
        GLAPI void            GLAPIENTRY glEvalCoord1fv (const GLfloat *u);
        GLAPI void            GLAPIENTRY glEvalCoord2d (GLdouble u, GLdouble v);
        GLAPI void            GLAPIENTRY glEvalCoord2dv (const GLdouble *u);
        GLAPI void            GLAPIENTRY glEvalCoord2f (GLfloat u, GLfloat v);
        GLAPI void            GLAPIENTRY glEvalCoord2fv (const GLfloat *u);
        GLAPI void            GLAPIENTRY glEvalMesh1 (GLenum mode, GLint i1, GLint i2);
        GLAPI void            GLAPIENTRY glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
        GLAPI void            GLAPIENTRY glEvalPoint1 (GLint i);
        GLAPI void            GLAPIENTRY glEvalPoint2 (GLint i, GLint j);
        GLAPI void            GLAPIENTRY glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer);
        GLAPI void            GLAPIENTRY glFinish (void);
        GLAPI void            GLAPIENTRY glFlush (void);
        GLAPI void            GLAPIENTRY glFogf (GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glFogfv (GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glFogi (GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glFogiv (GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glFrontFace (GLenum mode);
        GLAPI void            GLAPIENTRY glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
        GLAPI GLuint          GLAPIENTRY glGenLists (GLsizei range);
        GLAPI void            GLAPIENTRY glGenTextures (GLsizei n, GLuint *textures);
        GLAPI void            GLAPIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
        GLAPI void            GLAPIENTRY glGetClipPlane (GLenum plane, GLdouble *equation);
        GLAPI void            GLAPIENTRY glGetDoublev (GLenum pname, GLdouble *params);
        GLAPI GLenum          GLAPIENTRY glGetError (void);
        GLAPI void            GLAPIENTRY glGetFloatv (GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetIntegerv (GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetLightiv (GLenum light, GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glGetMapdv (GLenum target, GLenum query, GLdouble *v);
        GLAPI void            GLAPIENTRY glGetMapfv (GLenum target, GLenum query, GLfloat *v);
        GLAPI void            GLAPIENTRY glGetMapiv (GLenum target, GLenum query, GLint *v);
        GLAPI void            GLAPIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetMaterialiv (GLenum face, GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glGetPixelMapfv (GLenum map, GLfloat *values);
        GLAPI void            GLAPIENTRY glGetPixelMapuiv (GLenum map, GLuint *values);
        GLAPI void            GLAPIENTRY glGetPixelMapusv (GLenum map, GLushort *values);
        GLAPI void            GLAPIENTRY glGetPointerv (GLenum pname, void* *params);
        GLAPI void            GLAPIENTRY glGetPolygonStipple (GLubyte *mask);
        GLAPI const GLubyte * GLAPIENTRY glGetString (GLenum name);
        GLAPI void            GLAPIENTRY glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetTexEnviv (GLenum target, GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params);
        GLAPI void            GLAPIENTRY glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetTexGeniv (GLenum coord, GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
        GLAPI void            GLAPIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
        GLAPI void            GLAPIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
        GLAPI void            GLAPIENTRY glHint (GLenum target, GLenum mode);
        GLAPI void            GLAPIENTRY glIndexMask (GLuint mask);
        GLAPI void            GLAPIENTRY glIndexPointer (GLenum type, GLsizei stride, const void *pointer);
        GLAPI void            GLAPIENTRY glIndexd (GLdouble c);
        GLAPI void            GLAPIENTRY glIndexdv (const GLdouble *c);
        GLAPI void            GLAPIENTRY glIndexf (GLfloat c);
        GLAPI void            GLAPIENTRY glIndexfv (const GLfloat *c);
        GLAPI void            GLAPIENTRY glIndexi (GLint c);
        GLAPI void            GLAPIENTRY glIndexiv (const GLint *c);
        GLAPI void            GLAPIENTRY glIndexs (GLshort c);
        GLAPI void            GLAPIENTRY glIndexsv (const GLshort *c);
        GLAPI void            GLAPIENTRY glIndexub (GLubyte c);
        GLAPI void            GLAPIENTRY glIndexubv (const GLubyte *c);
        GLAPI void            GLAPIENTRY glInitNames (void);
        GLAPI void            GLAPIENTRY glInterleavedArrays (GLenum format, GLsizei stride, const void *pointer);
        GLAPI GLboolean       GLAPIENTRY glIsEnabled (GLenum cap);
        GLAPI GLboolean       GLAPIENTRY glIsList (GLuint list);
        GLAPI GLboolean       GLAPIENTRY glIsTexture (GLuint texture);
        GLAPI void            GLAPIENTRY glLightModelf (GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glLightModelfv (GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glLightModeli (GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glLightModeliv (GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glLightf (GLenum light, GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glLighti (GLenum light, GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glLightiv (GLenum light, GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glLineStipple (GLint factor, GLushort pattern);
        GLAPI void            GLAPIENTRY glLineWidth (GLfloat width);
        GLAPI void            GLAPIENTRY glListBase (GLuint base);
        GLAPI void            GLAPIENTRY glLoadIdentity (void);
        GLAPI void            GLAPIENTRY glLoadMatrixd (const GLdouble *m);
        GLAPI void            GLAPIENTRY glLoadMatrixf (const GLfloat *m);
        GLAPI void            GLAPIENTRY glLoadName (GLuint name);
        GLAPI void            GLAPIENTRY glLogicOp (GLenum opcode);
        GLAPI void            GLAPIENTRY glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
        GLAPI void            GLAPIENTRY glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
        GLAPI void            GLAPIENTRY glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
        GLAPI void            GLAPIENTRY glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
        GLAPI void            GLAPIENTRY glMapGrid1d (GLint un, GLdouble u1, GLdouble u2);
        GLAPI void            GLAPIENTRY glMapGrid1f (GLint un, GLfloat u1, GLfloat u2);
        GLAPI void            GLAPIENTRY glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
        GLAPI void            GLAPIENTRY glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
        GLAPI void            GLAPIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glMateriali (GLenum face, GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glMaterialiv (GLenum face, GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glMatrixMode (GLenum mode);
        GLAPI void            GLAPIENTRY glMultMatrixd (const GLdouble *m);
        GLAPI void            GLAPIENTRY glMultMatrixf (const GLfloat *m);
        GLAPI void            GLAPIENTRY glNewList (GLuint list, GLenum mode);
        GLAPI void            GLAPIENTRY glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz);
        GLAPI void            GLAPIENTRY glNormal3bv (const GLbyte *v);
        GLAPI void            GLAPIENTRY glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz);
        GLAPI void            GLAPIENTRY glNormal3dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz);
        GLAPI void            GLAPIENTRY glNormal3fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glNormal3i (GLint nx, GLint ny, GLint nz);
        GLAPI void            GLAPIENTRY glNormal3iv (const GLint *v);
        GLAPI void            GLAPIENTRY glNormal3s (GLshort nx, GLshort ny, GLshort nz);
        GLAPI void            GLAPIENTRY glNormal3sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glNormalPointer (GLenum type, GLsizei stride, const void *pointer);
        GLAPI void            GLAPIENTRY glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
        GLAPI void            GLAPIENTRY glPassThrough (GLfloat token);
        GLAPI void            GLAPIENTRY glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values);
        GLAPI void            GLAPIENTRY glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values);
        GLAPI void            GLAPIENTRY glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values);
        GLAPI void            GLAPIENTRY glPixelStoref (GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glPixelStorei (GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glPixelTransferf (GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glPixelTransferi (GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glPixelZoom (GLfloat xfactor, GLfloat yfactor);
        GLAPI void            GLAPIENTRY glPointSize (GLfloat size);
        GLAPI void            GLAPIENTRY glPolygonMode (GLenum face, GLenum mode);
        GLAPI void            GLAPIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
        GLAPI void            GLAPIENTRY glPolygonStipple (const GLubyte *mask);
        GLAPI void            GLAPIENTRY glPopAttrib (void);
        GLAPI void            GLAPIENTRY glPopClientAttrib (void);
        GLAPI void            GLAPIENTRY glPopMatrix (void);
        GLAPI void            GLAPIENTRY glPopName (void);
        GLAPI void            GLAPIENTRY glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities);
        GLAPI void            GLAPIENTRY glPushAttrib (GLbitfield mask);
        GLAPI void            GLAPIENTRY glPushClientAttrib (GLbitfield mask);
        GLAPI void            GLAPIENTRY glPushMatrix (void);
        GLAPI void            GLAPIENTRY glPushName (GLuint name);
        GLAPI void            GLAPIENTRY glRasterPos2d (GLdouble x, GLdouble y);
        GLAPI void            GLAPIENTRY glRasterPos2dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glRasterPos2f (GLfloat x, GLfloat y);
        GLAPI void            GLAPIENTRY glRasterPos2fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glRasterPos2i (GLint x, GLint y);
        GLAPI void            GLAPIENTRY glRasterPos2iv (const GLint *v);
        GLAPI void            GLAPIENTRY glRasterPos2s (GLshort x, GLshort y);
        GLAPI void            GLAPIENTRY glRasterPos2sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glRasterPos3d (GLdouble x, GLdouble y, GLdouble z);
        GLAPI void            GLAPIENTRY glRasterPos3dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glRasterPos3f (GLfloat x, GLfloat y, GLfloat z);
        GLAPI void            GLAPIENTRY glRasterPos3fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glRasterPos3i (GLint x, GLint y, GLint z);
        GLAPI void            GLAPIENTRY glRasterPos3iv (const GLint *v);
        GLAPI void            GLAPIENTRY glRasterPos3s (GLshort x, GLshort y, GLshort z);
        GLAPI void            GLAPIENTRY glRasterPos3sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
        GLAPI void            GLAPIENTRY glRasterPos4dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        GLAPI void            GLAPIENTRY glRasterPos4fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glRasterPos4i (GLint x, GLint y, GLint z, GLint w);
        GLAPI void            GLAPIENTRY glRasterPos4iv (const GLint *v);
        GLAPI void            GLAPIENTRY glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w);
        GLAPI void            GLAPIENTRY glRasterPos4sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glReadBuffer (GLenum mode);
        GLAPI void            GLAPIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
        GLAPI void            GLAPIENTRY glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
        GLAPI void            GLAPIENTRY glRectdv (const GLdouble *v1, const GLdouble *v2);
        GLAPI void            GLAPIENTRY glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
        GLAPI void            GLAPIENTRY glRectfv (const GLfloat *v1, const GLfloat *v2);
        GLAPI void            GLAPIENTRY glRecti (GLint x1, GLint y1, GLint x2, GLint y2);
        GLAPI void            GLAPIENTRY glRectiv (const GLint *v1, const GLint *v2);
        GLAPI void            GLAPIENTRY glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
        GLAPI void            GLAPIENTRY glRectsv (const GLshort *v1, const GLshort *v2);
        GLAPI GLint           GLAPIENTRY glRenderMode (GLenum mode);
        GLAPI void            GLAPIENTRY glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
        GLAPI void            GLAPIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
        GLAPI void            GLAPIENTRY glScaled (GLdouble x, GLdouble y, GLdouble z);
        GLAPI void            GLAPIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z);
        GLAPI void            GLAPIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
        GLAPI void            GLAPIENTRY glSelectBuffer (GLsizei size, GLuint *buffer);
        GLAPI void            GLAPIENTRY glShadeModel (GLenum mode);
        GLAPI void            GLAPIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
        GLAPI void            GLAPIENTRY glStencilMask (GLuint mask);
        GLAPI void            GLAPIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
        GLAPI void            GLAPIENTRY glTexCoord1d (GLdouble s);
        GLAPI void            GLAPIENTRY glTexCoord1dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glTexCoord1f (GLfloat s);
        GLAPI void            GLAPIENTRY glTexCoord1fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glTexCoord1i (GLint s);
        GLAPI void            GLAPIENTRY glTexCoord1iv (const GLint *v);
        GLAPI void            GLAPIENTRY glTexCoord1s (GLshort s);
        GLAPI void            GLAPIENTRY glTexCoord1sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glTexCoord2d (GLdouble s, GLdouble t);
        GLAPI void            GLAPIENTRY glTexCoord2dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glTexCoord2f (GLfloat s, GLfloat t);
        GLAPI void            GLAPIENTRY glTexCoord2fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glTexCoord2i (GLint s, GLint t);
        GLAPI void            GLAPIENTRY glTexCoord2iv (const GLint *v);
        GLAPI void            GLAPIENTRY glTexCoord2s (GLshort s, GLshort t);
        GLAPI void            GLAPIENTRY glTexCoord2sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glTexCoord3d (GLdouble s, GLdouble t, GLdouble r);
        GLAPI void            GLAPIENTRY glTexCoord3dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glTexCoord3f (GLfloat s, GLfloat t, GLfloat r);
        GLAPI void            GLAPIENTRY glTexCoord3fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glTexCoord3i (GLint s, GLint t, GLint r);
        GLAPI void            GLAPIENTRY glTexCoord3iv (const GLint *v);
        GLAPI void            GLAPIENTRY glTexCoord3s (GLshort s, GLshort t, GLshort r);
        GLAPI void            GLAPIENTRY glTexCoord3sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
        GLAPI void            GLAPIENTRY glTexCoord4dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
        GLAPI void            GLAPIENTRY glTexCoord4fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glTexCoord4i (GLint s, GLint t, GLint r, GLint q);
        GLAPI void            GLAPIENTRY glTexCoord4iv (const GLint *v);
        GLAPI void            GLAPIENTRY glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q);
        GLAPI void            GLAPIENTRY glTexCoord4sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const void *pointer);
        GLAPI void            GLAPIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glTexGend (GLenum coord, GLenum pname, GLdouble param);
        GLAPI void            GLAPIENTRY glTexGendv (GLenum coord, GLenum pname, const GLdouble *params);
        GLAPI void            GLAPIENTRY glTexGenf (GLenum coord, GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glTexGeni (GLenum coord, GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glTexGeniv (GLenum coord, GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
        GLAPI void            GLAPIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
        GLAPI void            GLAPIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
        GLAPI void            GLAPIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
        GLAPI void            GLAPIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
        GLAPI void            GLAPIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
        GLAPI void            GLAPIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
        GLAPI void            GLAPIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
        GLAPI void            GLAPIENTRY glTranslated (GLdouble x, GLdouble y, GLdouble z);
        GLAPI void            GLAPIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z);
        GLAPI void            GLAPIENTRY glVertex2d (GLdouble x, GLdouble y);
        GLAPI void            GLAPIENTRY glVertex2dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glVertex2f (GLfloat x, GLfloat y);
        GLAPI void            GLAPIENTRY glVertex2fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glVertex2i (GLint x, GLint y);
        GLAPI void            GLAPIENTRY glVertex2iv (const GLint *v);
        GLAPI void            GLAPIENTRY glVertex2s (GLshort x, GLshort y);
        GLAPI void            GLAPIENTRY glVertex2sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glVertex3d (GLdouble x, GLdouble y, GLdouble z);
        GLAPI void            GLAPIENTRY glVertex3dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glVertex3f (GLfloat x, GLfloat y, GLfloat z);
        GLAPI void            GLAPIENTRY glVertex3fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glVertex3i (GLint x, GLint y, GLint z);
        GLAPI void            GLAPIENTRY glVertex3iv (const GLint *v);
        GLAPI void            GLAPIENTRY glVertex3s (GLshort x, GLshort y, GLshort z);
        GLAPI void            GLAPIENTRY glVertex3sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
        GLAPI void            GLAPIENTRY glVertex4dv (const GLdouble *v);
        GLAPI void            GLAPIENTRY glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        GLAPI void            GLAPIENTRY glVertex4fv (const GLfloat *v);
        GLAPI void            GLAPIENTRY glVertex4i (GLint x, GLint y, GLint z, GLint w);
        GLAPI void            GLAPIENTRY glVertex4iv (const GLint *v);
        GLAPI void            GLAPIENTRY glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w);
        GLAPI void            GLAPIENTRY glVertex4sv (const GLshort *v);
        GLAPI void            GLAPIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const void *pointer);
        GLAPI void            GLAPIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
    
  #endif // GLE_HOOKING_ENABLED

#endif // GL_VERSION_1_1




// OpenGL 1.2+ functions are not declared in Microsoft's gl.h

#ifndef GL_VERSION_1_2
    #define GL_VERSION_1_2 1

    #define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
    #define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
    #define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
    #define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
    #define GL_UNSIGNED_BYTE_3_3_2 0x8032
    #define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
    #define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
    #define GL_UNSIGNED_INT_8_8_8_8 0x8035
    #define GL_UNSIGNED_INT_10_10_10_2 0x8036
    #define GL_RESCALE_NORMAL 0x803A
    #define GL_TEXTURE_BINDING_3D 0x806A
    #define GL_PACK_SKIP_IMAGES 0x806B
    #define GL_PACK_IMAGE_HEIGHT 0x806C
    #define GL_UNPACK_SKIP_IMAGES 0x806D
    #define GL_UNPACK_IMAGE_HEIGHT 0x806E
    #define GL_TEXTURE_3D 0x806F
    #define GL_PROXY_TEXTURE_3D 0x8070
    #define GL_TEXTURE_DEPTH 0x8071
    #define GL_TEXTURE_WRAP_R 0x8072
    #define GL_MAX_3D_TEXTURE_SIZE 0x8073
    #define GL_BGR 0x80E0
    #define GL_BGRA 0x80E1
    #define GL_MAX_ELEMENTS_VERTICES 0x80E8
    #define GL_MAX_ELEMENTS_INDICES 0x80E9
    #define GL_CLAMP_TO_EDGE 0x812F
    #define GL_TEXTURE_MIN_LOD 0x813A
    #define GL_TEXTURE_MAX_LOD 0x813B
    #define GL_TEXTURE_BASE_LEVEL 0x813C
    #define GL_TEXTURE_MAX_LEVEL 0x813D
    #define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
    #define GL_SINGLE_COLOR 0x81F9
    #define GL_SEPARATE_SPECULAR_COLOR 0x81FA
    #define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
    #define GL_UNSIGNED_SHORT_5_6_5 0x8363
    #define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
    #define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
    #define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
    #define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
    #define GL_ALIASED_POINT_SIZE_RANGE 0x846D
    #define GL_ALIASED_LINE_WIDTH_RANGE 0x846E

    typedef void (GLAPIENTRY * PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY * PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
    typedef void (GLAPIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
    typedef void (GLAPIENTRY * PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);

    #define glCopyTexSubImage3D GLEGetCurrentFunction(glCopyTexSubImage3D)
    #define glDrawRangeElements GLEGetCurrentFunction(glDrawRangeElements)
    #define glTexImage3D        GLEGetCurrentFunction(glTexImage3D)
    #define glTexSubImage3D     GLEGetCurrentFunction(glTexSubImage3D)

    // OpenGL 2.1 deprecated functions
    /*
    typedef void (GLAPIENTRY PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    typedef void (GLAPIENTRY PFNGLBLENDEQUATIONPROC) (GLenum mode);
    typedef void (GLAPIENTRY PFNGLCOLORTABLEPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
    typedef void (GLAPIENTRY PFNGLCOLORTABLEPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
    typedef void (GLAPIENTRY PFNGLCOLORTABLEPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
    typedef void (GLAPIENTRY PFNGLCOPYCOLORTABLEPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
    typedef void (GLAPIENTRY PFNGLGETCOLORTABLEPROC) (GLenum target, GLenum format, GLenum type, GLvoid *table);
    typedef void (GLAPIENTRY PFNGLGETCOLORTABLEPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    typedef void (GLAPIENTRY PFNGLGETCOLORTABLEPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (GLAPIENTRY PFNGLCOLORSUBTABLEPROC) (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
    typedef void (GLAPIENTRY PFNGLCOPYCOLORSUBTABLEPROC) (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);
    typedef void (GLAPIENTRY PFNGLCONVOLUTIONFILTER1DPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
    typedef void (GLAPIENTRY PFNGLCONVOLUTIONFILTER2DPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
    typedef void (GLAPIENTRY PFNGLCONVOLUTIONPARAMETERFPROC) (GLenum target, GLenum pname, GLfloat params);
    typedef void (GLAPIENTRY PFNGLCONVOLUTIONPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
    typedef void (GLAPIENTRY PFNGLCONVOLUTIONPARAMETERIPROC) (GLenum target, GLenum pname, GLint params);
    typedef void (GLAPIENTRY PFNGLCONVOLUTIONPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
    typedef void (GLAPIENTRY PFNGLCOPYCONVOLUTIONFILTER1DPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
    typedef void (GLAPIENTRY PFNGLCOPYCONVOLUTIONFILTER2DPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY PFNGLGETCONVOLUTIONFILTERPROC) (GLenum target, GLenum format, GLenum type, GLvoid *image);
    typedef void (GLAPIENTRY PFNGLGETCONVOLUTIONPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    typedef void (GLAPIENTRY PFNGLGETCONVOLUTIONPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (GLAPIENTRY PFNGLGETSEPARABLEFILTERPROC) (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
    typedef void (GLAPIENTRY PFNGLSEPARABLEFILTER2DPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);
    typedef void (GLAPIENTRY PFNGLGETHISTOGRAMPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
    typedef void (GLAPIENTRY PFNGLGETHISTOGRAMPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    typedef void (GLAPIENTRY PFNGLGETHISTOGRAMPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (GLAPIENTRY PFNGLGETMINMAXPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
    typedef void (GLAPIENTRY PFNGLGETMINMAXPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    typedef void (GLAPIENTRY PFNGLGETMINMAXPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (GLAPIENTRY PFNGLHISTOGRAMPROC) (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
    typedef void (GLAPIENTRY PFNGLMINMAXPROC) (GLenum target, GLenum internalformat, GLboolean sink);
    typedef void (GLAPIENTRY PFNGLRESETHISTOGRAMPROC) (GLenum target);
    typedef void (GLAPIENTRY PFNGLRESETMINMAXPROC) (GLenum target);
    */
#endif // GL_VERSION_1_2



#ifndef GL_VERSION_1_3
    #define GL_VERSION_1_3 1

    #define GL_MULTISAMPLE 0x809D
    #define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
    #define GL_SAMPLE_ALPHA_TO_ONE 0x809F
    #define GL_SAMPLE_COVERAGE 0x80A0
    #define GL_SAMPLE_BUFFERS 0x80A8
    #define GL_SAMPLES 0x80A9
    #define GL_SAMPLE_COVERAGE_VALUE 0x80AA
    #define GL_SAMPLE_COVERAGE_INVERT 0x80AB
    #define GL_CLAMP_TO_BORDER 0x812D
    #define GL_TEXTURE0 0x84C0
    #define GL_TEXTURE1 0x84C1
    #define GL_TEXTURE2 0x84C2
    #define GL_TEXTURE3 0x84C3
    #define GL_TEXTURE4 0x84C4
    #define GL_TEXTURE5 0x84C5
    #define GL_TEXTURE6 0x84C6
    #define GL_TEXTURE7 0x84C7
    #define GL_TEXTURE8 0x84C8
    #define GL_TEXTURE9 0x84C9
    #define GL_TEXTURE10 0x84CA
    #define GL_TEXTURE11 0x84CB
    #define GL_TEXTURE12 0x84CC
    #define GL_TEXTURE13 0x84CD
    #define GL_TEXTURE14 0x84CE
    #define GL_TEXTURE15 0x84CF
    #define GL_TEXTURE16 0x84D0
    #define GL_TEXTURE17 0x84D1
    #define GL_TEXTURE18 0x84D2
    #define GL_TEXTURE19 0x84D3
    #define GL_TEXTURE20 0x84D4
    #define GL_TEXTURE21 0x84D5
    #define GL_TEXTURE22 0x84D6
    #define GL_TEXTURE23 0x84D7
    #define GL_TEXTURE24 0x84D8
    #define GL_TEXTURE25 0x84D9
    #define GL_TEXTURE26 0x84DA
    #define GL_TEXTURE27 0x84DB
    #define GL_TEXTURE28 0x84DC
    #define GL_TEXTURE29 0x84DD
    #define GL_TEXTURE30 0x84DE
    #define GL_TEXTURE31 0x84DF
    #define GL_ACTIVE_TEXTURE 0x84E0
    #define GL_CLIENT_ACTIVE_TEXTURE 0x84E1
    #define GL_MAX_TEXTURE_UNITS 0x84E2
    #define GL_TRANSPOSE_MODELVIEW_MATRIX 0x84E3
    #define GL_TRANSPOSE_PROJECTION_MATRIX 0x84E4
    #define GL_TRANSPOSE_TEXTURE_MATRIX 0x84E5
    #define GL_TRANSPOSE_COLOR_MATRIX 0x84E6
    #define GL_SUBTRACT 0x84E7
    #define GL_COMPRESSED_ALPHA 0x84E9
    #define GL_COMPRESSED_LUMINANCE 0x84EA
    #define GL_COMPRESSED_LUMINANCE_ALPHA 0x84EB
    #define GL_COMPRESSED_INTENSITY 0x84EC
    #define GL_COMPRESSED_RGB 0x84ED
    #define GL_COMPRESSED_RGBA 0x84EE
    #define GL_TEXTURE_COMPRESSION_HINT 0x84EF
    #define GL_NORMAL_MAP 0x8511
    #define GL_REFLECTION_MAP 0x8512
    #define GL_TEXTURE_CUBE_MAP 0x8513
    #define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
    #define GL_PROXY_TEXTURE_CUBE_MAP 0x851B
    #define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
    #define GL_COMBINE 0x8570
    #define GL_COMBINE_RGB 0x8571
    #define GL_COMBINE_ALPHA 0x8572
    #define GL_RGB_SCALE 0x8573
    #define GL_ADD_SIGNED 0x8574
    #define GL_INTERPOLATE 0x8575
    #define GL_CONSTANT 0x8576
    #define GL_PRIMARY_COLOR 0x8577
    #define GL_PREVIOUS 0x8578
    #define GL_SOURCE0_RGB 0x8580
    #define GL_SOURCE1_RGB 0x8581
    #define GL_SOURCE2_RGB 0x8582
    #define GL_SOURCE0_ALPHA 0x8588
    #define GL_SOURCE1_ALPHA 0x8589
    #define GL_SOURCE2_ALPHA 0x858A
    #define GL_OPERAND0_RGB 0x8590
    #define GL_OPERAND1_RGB 0x8591
    #define GL_OPERAND2_RGB 0x8592
    #define GL_OPERAND0_ALPHA 0x8598
    #define GL_OPERAND1_ALPHA 0x8599
    #define GL_OPERAND2_ALPHA 0x859A
    #define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
    #define GL_TEXTURE_COMPRESSED 0x86A1
    #define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
    #define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
    #define GL_DOT3_RGB 0x86AE
    #define GL_DOT3_RGBA 0x86AF
    #define GL_MULTISAMPLE_BIT 0x20000000

    typedef void (GLAPIENTRY * PFNGLACTIVETEXTUREPROC) (GLenum texture);
    typedef void (GLAPIENTRY * PFNGLCLIENTACTIVETEXTUREPROC) (GLenum texture);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint lod, void *img);
    typedef void (GLAPIENTRY * PFNGLLOADTRANSPOSEMATRIXDPROC) (const GLdouble m[16]);
    typedef void (GLAPIENTRY * PFNGLLOADTRANSPOSEMATRIXFPROC) (const GLfloat m[16]);
    typedef void (GLAPIENTRY * PFNGLMULTTRANSPOSEMATRIXDPROC) (const GLdouble m[16]);
    typedef void (GLAPIENTRY * PFNGLMULTTRANSPOSEMATRIXFPROC) (const GLfloat m[16]);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1DPROC) (GLenum target, GLdouble s);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1DVPROC) (GLenum target, const GLdouble *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1FPROC) (GLenum target, GLfloat s);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1FVPROC) (GLenum target, const GLfloat *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1IPROC) (GLenum target, GLint s);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1IVPROC) (GLenum target, const GLint *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1SPROC) (GLenum target, GLshort s);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD1SVPROC) (GLenum target, const GLshort *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2DPROC) (GLenum target, GLdouble s, GLdouble t);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2DVPROC) (GLenum target, const GLdouble *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2FPROC) (GLenum target, GLfloat s, GLfloat t);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2FVPROC) (GLenum target, const GLfloat *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2IPROC) (GLenum target, GLint s, GLint t);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2IVPROC) (GLenum target, const GLint *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2SPROC) (GLenum target, GLshort s, GLshort t);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD2SVPROC) (GLenum target, const GLshort *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3DPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3DVPROC) (GLenum target, const GLdouble *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3FPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3FVPROC) (GLenum target, const GLfloat *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3IPROC) (GLenum target, GLint s, GLint t, GLint r);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3IVPROC) (GLenum target, const GLint *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3SPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD3SVPROC) (GLenum target, const GLshort *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4DPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4DVPROC) (GLenum target, const GLdouble *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4FPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4FVPROC) (GLenum target, const GLfloat *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4IPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4IVPROC) (GLenum target, const GLint *v);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4SPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
    typedef void (GLAPIENTRY * PFNGLMULTITEXCOORD4SVPROC) (GLenum target, const GLshort *v);
    typedef void (GLAPIENTRY * PFNGLSAMPLECOVERAGEPROC) (GLclampf value, GLboolean invert);

    #define glActiveTexture             GLEGetCurrentFunction(glActiveTexture)
    #define glClientActiveTexture       GLEGetCurrentFunction(glClientActiveTexture)
    #define glCompressedTexImage1D      GLEGetCurrentFunction(glCompressedTexImage1D)
    #define glCompressedTexImage2D      GLEGetCurrentFunction(glCompressedTexImage2D)
    #define glCompressedTexImage3D      GLEGetCurrentFunction(glCompressedTexImage3D)
    #define glCompressedTexSubImage1D   GLEGetCurrentFunction(glCompressedTexSubImage1D)
    #define glCompressedTexSubImage2D   GLEGetCurrentFunction(glCompressedTexSubImage2D)
    #define glCompressedTexSubImage3D   GLEGetCurrentFunction(glCompressedTexSubImage3D)
    #define glGetCompressedTexImage     GLEGetCurrentFunction(glGetCompressedTexImage)
    #define glLoadTransposeMatrixd      GLEGetCurrentFunction(glLoadTransposeMatrixd)
    #define glLoadTransposeMatrixf      GLEGetCurrentFunction(glLoadTransposeMatrixf)
    #define glMultTransposeMatrixd      GLEGetCurrentFunction(glMultTransposeMatrixd)
    #define glMultTransposeMatrixf      GLEGetCurrentFunction(glMultTransposeMatrixf)
    #define glMultiTexCoord1d           GLEGetCurrentFunction(glMultiTexCoord1d)
    #define glMultiTexCoord1dv          GLEGetCurrentFunction(glMultiTexCoord1dv)
    #define glMultiTexCoord1f           GLEGetCurrentFunction(glMultiTexCoord1f)
    #define glMultiTexCoord1fv          GLEGetCurrentFunction(glMultiTexCoord1fv)
    #define glMultiTexCoord1i           GLEGetCurrentFunction(glMultiTexCoord1i)
    #define glMultiTexCoord1iv          GLEGetCurrentFunction(glMultiTexCoord1iv)
    #define glMultiTexCoord1s           GLEGetCurrentFunction(glMultiTexCoord1s)
    #define glMultiTexCoord1sv          GLEGetCurrentFunction(glMultiTexCoord1sv)
    #define glMultiTexCoord2d           GLEGetCurrentFunction(glMultiTexCoord2d)
    #define glMultiTexCoord2dv          GLEGetCurrentFunction(glMultiTexCoord2dv)
    #define glMultiTexCoord2f           GLEGetCurrentFunction(glMultiTexCoord2f)
    #define glMultiTexCoord2fv          GLEGetCurrentFunction(glMultiTexCoord2fv)
    #define glMultiTexCoord2i           GLEGetCurrentFunction(glMultiTexCoord2i)
    #define glMultiTexCoord2iv          GLEGetCurrentFunction(glMultiTexCoord2iv)
    #define glMultiTexCoord2s           GLEGetCurrentFunction(glMultiTexCoord2s)
    #define glMultiTexCoord2sv          GLEGetCurrentFunction(glMultiTexCoord2sv)
    #define glMultiTexCoord3d           GLEGetCurrentFunction(glMultiTexCoord3d)
    #define glMultiTexCoord3dv          GLEGetCurrentFunction(glMultiTexCoord3dv)
    #define glMultiTexCoord3f           GLEGetCurrentFunction(glMultiTexCoord3f)
    #define glMultiTexCoord3fv          GLEGetCurrentFunction(glMultiTexCoord3fv)
    #define glMultiTexCoord3i           GLEGetCurrentFunction(glMultiTexCoord3i)
    #define glMultiTexCoord3iv          GLEGetCurrentFunction(glMultiTexCoord3iv)
    #define glMultiTexCoord3s           GLEGetCurrentFunction(glMultiTexCoord3s)
    #define glMultiTexCoord3sv          GLEGetCurrentFunction(glMultiTexCoord3sv)
    #define glMultiTexCoord4d           GLEGetCurrentFunction(glMultiTexCoord4d)
    #define glMultiTexCoord4dv          GLEGetCurrentFunction(glMultiTexCoord4dv)
    #define glMultiTexCoord4f           GLEGetCurrentFunction(glMultiTexCoord4f)
    #define glMultiTexCoord4fv          GLEGetCurrentFunction(glMultiTexCoord4fv)
    #define glMultiTexCoord4i           GLEGetCurrentFunction(glMultiTexCoord4i)
    #define glMultiTexCoord4iv          GLEGetCurrentFunction(glMultiTexCoord4iv)
    #define glMultiTexCoord4s           GLEGetCurrentFunction(glMultiTexCoord4s)
    #define glMultiTexCoord4sv          GLEGetCurrentFunction(glMultiTexCoord4sv)
    #define glSampleCoverage            GLEGetCurrentFunction(glSampleCoverage)

#endif // GL_VERSION_1_3



#ifndef GL_VERSION_1_4
    #define GL_VERSION_1_4 1

    #define GL_BLEND_DST_RGB 0x80C8
    #define GL_BLEND_SRC_RGB 0x80C9
    #define GL_BLEND_DST_ALPHA 0x80CA
    #define GL_BLEND_SRC_ALPHA 0x80CB
    #define GL_POINT_SIZE_MIN 0x8126
    #define GL_POINT_SIZE_MAX 0x8127
    #define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
    #define GL_POINT_DISTANCE_ATTENUATION 0x8129
    #define GL_GENERATE_MIPMAP 0x8191
    #define GL_GENERATE_MIPMAP_HINT 0x8192
    #define GL_DEPTH_COMPONENT16 0x81A5
    #define GL_DEPTH_COMPONENT24 0x81A6
    #define GL_DEPTH_COMPONENT32 0x81A7
    #define GL_MIRRORED_REPEAT 0x8370
    #define GL_FOG_COORDINATE_SOURCE 0x8450
    #define GL_FOG_COORDINATE 0x8451
    #define GL_FRAGMENT_DEPTH 0x8452
    #define GL_CURRENT_FOG_COORDINATE 0x8453
    #define GL_FOG_COORDINATE_ARRAY_TYPE 0x8454
    #define GL_FOG_COORDINATE_ARRAY_STRIDE 0x8455
    #define GL_FOG_COORDINATE_ARRAY_POINTER 0x8456
    #define GL_FOG_COORDINATE_ARRAY 0x8457
    #define GL_COLOR_SUM 0x8458
    #define GL_CURRENT_SECONDARY_COLOR 0x8459
    #define GL_SECONDARY_COLOR_ARRAY_SIZE 0x845A
    #define GL_SECONDARY_COLOR_ARRAY_TYPE 0x845B
    #define GL_SECONDARY_COLOR_ARRAY_STRIDE 0x845C
    #define GL_SECONDARY_COLOR_ARRAY_POINTER 0x845D
    #define GL_SECONDARY_COLOR_ARRAY 0x845E
    #define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
    #define GL_TEXTURE_FILTER_CONTROL 0x8500
    #define GL_TEXTURE_LOD_BIAS 0x8501
    #define GL_INCR_WRAP 0x8507
    #define GL_DECR_WRAP 0x8508
    #define GL_TEXTURE_DEPTH_SIZE 0x884A
    #define GL_DEPTH_TEXTURE_MODE 0x884B
    #define GL_TEXTURE_COMPARE_MODE 0x884C
    #define GL_TEXTURE_COMPARE_FUNC 0x884D
    #define GL_COMPARE_R_TO_TEXTURE 0x884E

    typedef void (GLAPIENTRY * PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    typedef void (GLAPIENTRY * PFNGLBLENDEQUATIONPROC) (GLenum mode);
    typedef void (GLAPIENTRY * PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
    typedef void (GLAPIENTRY * PFNGLFOGCOORDPOINTERPROC) (GLenum type, GLsizei stride, const void *pointer);
    typedef void (GLAPIENTRY * PFNGLFOGCOORDDPROC) (GLdouble coord);
    typedef void (GLAPIENTRY * PFNGLFOGCOORDDVPROC) (const GLdouble *coord);
    typedef void (GLAPIENTRY * PFNGLFOGCOORDFPROC) (GLfloat coord);
    typedef void (GLAPIENTRY * PFNGLFOGCOORDFVPROC) (const GLfloat *coord);
    typedef void (GLAPIENTRY * PFNGLMULTIDRAWARRAYSPROC) (GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
    typedef void (GLAPIENTRY * PFNGLMULTIDRAWELEMENTSPROC) (GLenum mode, const GLsizei *count, GLenum type, const void *const* indices, GLsizei drawcount);
    typedef void (GLAPIENTRY * PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);
    typedef void (GLAPIENTRY * PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
    typedef void (GLAPIENTRY * PFNGLPOINTPARAMETERIPROC) (GLenum pname, GLint param);
    typedef void (GLAPIENTRY * PFNGLPOINTPARAMETERIVPROC) (GLenum pname, const GLint *params);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3BPROC) (GLbyte red, GLbyte green, GLbyte blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3BVPROC) (const GLbyte *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3DPROC) (GLdouble red, GLdouble green, GLdouble blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3DVPROC) (const GLdouble *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3FPROC) (GLfloat red, GLfloat green, GLfloat blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3FVPROC) (const GLfloat *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3IPROC) (GLint red, GLint green, GLint blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3IVPROC) (const GLint *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3SPROC) (GLshort red, GLshort green, GLshort blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3SVPROC) (const GLshort *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3UBPROC) (GLubyte red, GLubyte green, GLubyte blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3UBVPROC) (const GLubyte *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3UIPROC) (GLuint red, GLuint green, GLuint blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3UIVPROC) (const GLuint *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3USPROC) (GLushort red, GLushort green, GLushort blue);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLOR3USVPROC) (const GLushort *v);
    typedef void (GLAPIENTRY * PFNGLSECONDARYCOLORPOINTERPROC) (GLint size, GLenum type, GLsizei stride, const void *pointer);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2DPROC) (GLdouble x, GLdouble y);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2DVPROC) (const GLdouble *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2FPROC) (GLfloat x, GLfloat y);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2FVPROC) (const GLfloat *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2IPROC) (GLint x, GLint y);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2IVPROC) (const GLint *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2SPROC) (GLshort x, GLshort y);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS2SVPROC) (const GLshort *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3DPROC) (GLdouble x, GLdouble y, GLdouble z);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3DVPROC) (const GLdouble *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3FPROC) (GLfloat x, GLfloat y, GLfloat z);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3FVPROC) (const GLfloat *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3IPROC) (GLint x, GLint y, GLint z);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3IVPROC) (const GLint *p);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3SPROC) (GLshort x, GLshort y, GLshort z);
    typedef void (GLAPIENTRY * PFNGLWINDOWPOS3SVPROC) (const GLshort *p);

    #define glBlendColor GLEGetCurrentFunction(glBlendColor)
    #define glBlendEquation GLEGetCurrentFunction(glBlendEquation)
    #define glBlendFuncSeparate GLEGetCurrentFunction(glBlendFuncSeparate)
    #define glFogCoordPointer GLEGetCurrentFunction(glFogCoordPointer)
    #define glFogCoordd GLEGetCurrentFunction(glFogCoordd)
    #define glFogCoorddv GLEGetCurrentFunction(glFogCoorddv)
    #define glFogCoordf GLEGetCurrentFunction(glFogCoordf)
    #define glFogCoordfv GLEGetCurrentFunction(glFogCoordfv)
    #define glMultiDrawArrays GLEGetCurrentFunction(glMultiDrawArrays)
    #define glMultiDrawElements GLEGetCurrentFunction(glMultiDrawElements)
    #define glPointParameterf GLEGetCurrentFunction(glPointParameterf)
    #define glPointParameterfv GLEGetCurrentFunction(glPointParameterfv)
    #define glPointParameteri GLEGetCurrentFunction(glPointParameteri)
    #define glPointParameteriv GLEGetCurrentFunction(glPointParameteriv)
    #define glSecondaryColor3b GLEGetCurrentFunction(glSecondaryColor3b)
    #define glSecondaryColor3bv GLEGetCurrentFunction(glSecondaryColor3bv)
    #define glSecondaryColor3d GLEGetCurrentFunction(glSecondaryColor3d)
    #define glSecondaryColor3dv GLEGetCurrentFunction(glSecondaryColor3dv)
    #define glSecondaryColor3f GLEGetCurrentFunction(glSecondaryColor3f)
    #define glSecondaryColor3fv GLEGetCurrentFunction(glSecondaryColor3fv)
    #define glSecondaryColor3i GLEGetCurrentFunction(glSecondaryColor3i)
    #define glSecondaryColor3iv GLEGetCurrentFunction(glSecondaryColor3iv)
    #define glSecondaryColor3s GLEGetCurrentFunction(glSecondaryColor3s)
    #define glSecondaryColor3sv GLEGetCurrentFunction(glSecondaryColor3sv)
    #define glSecondaryColor3ub GLEGetCurrentFunction(glSecondaryColor3ub)
    #define glSecondaryColor3ubv GLEGetCurrentFunction(glSecondaryColor3ubv)
    #define glSecondaryColor3ui GLEGetCurrentFunction(glSecondaryColor3ui)
    #define glSecondaryColor3uiv GLEGetCurrentFunction(glSecondaryColor3uiv)
    #define glSecondaryColor3us GLEGetCurrentFunction(glSecondaryColor3us)
    #define glSecondaryColor3usv GLEGetCurrentFunction(glSecondaryColor3usv)
    #define glSecondaryColorPointer GLEGetCurrentFunction(glSecondaryColorPointer)
    #define glWindowPos2d GLEGetCurrentFunction(glWindowPos2d)
    #define glWindowPos2dv GLEGetCurrentFunction(glWindowPos2dv)
    #define glWindowPos2f GLEGetCurrentFunction(glWindowPos2f)
    #define glWindowPos2fv GLEGetCurrentFunction(glWindowPos2fv)
    #define glWindowPos2i GLEGetCurrentFunction(glWindowPos2i)
    #define glWindowPos2iv GLEGetCurrentFunction(glWindowPos2iv)
    #define glWindowPos2s GLEGetCurrentFunction(glWindowPos2s)
    #define glWindowPos2sv GLEGetCurrentFunction(glWindowPos2sv)
    #define glWindowPos3d GLEGetCurrentFunction(glWindowPos3d)
    #define glWindowPos3dv GLEGetCurrentFunction(glWindowPos3dv)
    #define glWindowPos3f GLEGetCurrentFunction(glWindowPos3f)
    #define glWindowPos3fv GLEGetCurrentFunction(glWindowPos3fv)
    #define glWindowPos3i GLEGetCurrentFunction(glWindowPos3i)
    #define glWindowPos3iv GLEGetCurrentFunction(glWindowPos3iv)
    #define glWindowPos3s GLEGetCurrentFunction(glWindowPos3s)
    #define glWindowPos3sv GLEGetCurrentFunction(glWindowPos3sv)

#endif // GL_VERSION_1_4



#ifndef GL_VERSION_1_5
    #define GL_VERSION_1_5 1

    #define GL_CURRENT_FOG_COORD GL_CURRENT_FOG_COORDINATE
    #define GL_FOG_COORD GL_FOG_COORDINATE
    #define GL_FOG_COORD_ARRAY GL_FOG_COORDINATE_ARRAY
    #define GL_FOG_COORD_ARRAY_BUFFER_BINDING GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING
    #define GL_FOG_COORD_ARRAY_POINTER GL_FOG_COORDINATE_ARRAY_POINTER
    #define GL_FOG_COORD_ARRAY_STRIDE GL_FOG_COORDINATE_ARRAY_STRIDE
    #define GL_FOG_COORD_ARRAY_TYPE GL_FOG_COORDINATE_ARRAY_TYPE
    #define GL_FOG_COORD_SRC GL_FOG_COORDINATE_SOURCE
    #define GL_SRC0_ALPHA GL_SOURCE0_ALPHA
    #define GL_SRC0_RGB GL_SOURCE0_RGB
    #define GL_SRC1_ALPHA GL_SOURCE1_ALPHA
    #define GL_SRC1_RGB GL_SOURCE1_RGB
    #define GL_SRC2_ALPHA GL_SOURCE2_ALPHA
    #define GL_SRC2_RGB GL_SOURCE2_RGB
    #define GL_BUFFER_SIZE 0x8764
    #define GL_BUFFER_USAGE 0x8765
    #define GL_QUERY_COUNTER_BITS 0x8864
    #define GL_CURRENT_QUERY 0x8865
    #define GL_QUERY_RESULT 0x8866
    #define GL_QUERY_RESULT_AVAILABLE 0x8867
    #define GL_ARRAY_BUFFER 0x8892
    #define GL_ELEMENT_ARRAY_BUFFER 0x8893
    #define GL_ARRAY_BUFFER_BINDING 0x8894
    #define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
    #define GL_VERTEX_ARRAY_BUFFER_BINDING 0x8896
    #define GL_NORMAL_ARRAY_BUFFER_BINDING 0x8897
    #define GL_COLOR_ARRAY_BUFFER_BINDING 0x8898
    #define GL_INDEX_ARRAY_BUFFER_BINDING 0x8899
    #define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
    #define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
    #define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
    #define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
    #define GL_WEIGHT_ARRAY_BUFFER_BINDING 0x889E
    #define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
    #define GL_READ_ONLY 0x88B8
    #define GL_WRITE_ONLY 0x88B9
    #define GL_READ_WRITE 0x88BA
    #define GL_BUFFER_ACCESS 0x88BB
    #define GL_BUFFER_MAPPED 0x88BC
    #define GL_BUFFER_MAP_POINTER 0x88BD
    #define GL_STREAM_DRAW 0x88E0
    #define GL_STREAM_READ 0x88E1
    #define GL_STREAM_COPY 0x88E2
    #define GL_STATIC_DRAW 0x88E4
    #define GL_STATIC_READ 0x88E5
    #define GL_STATIC_COPY 0x88E6
    #define GL_DYNAMIC_DRAW 0x88E8
    #define GL_DYNAMIC_READ 0x88E9
    #define GL_DYNAMIC_COPY 0x88EA
    #define GL_SAMPLES_PASSED 0x8914

    typedef ptrdiff_t GLintptr;
    typedef ptrdiff_t GLsizeiptr;

    typedef void      (GLAPIENTRY * PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
    typedef void      (GLAPIENTRY * PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
    typedef void      (GLAPIENTRY * PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    typedef void      (GLAPIENTRY * PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
    typedef void      (GLAPIENTRY * PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint* buffers);
    typedef void      (GLAPIENTRY * PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint* ids);
    typedef void      (GLAPIENTRY * PFNGLENDQUERYPROC) (GLenum target);
    typedef void      (GLAPIENTRY * PFNGLGENBUFFERSPROC) (GLsizei n, GLuint* buffers);
    typedef void      (GLAPIENTRY * PFNGLGENQUERIESPROC) (GLsizei n, GLuint* ids);
    typedef void      (GLAPIENTRY * PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint* params);
    typedef void      (GLAPIENTRY * PFNGLGETBUFFERPOINTERVPROC) (GLenum target, GLenum pname, void** params);
    typedef void      (GLAPIENTRY * PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, void* data);
    typedef void      (GLAPIENTRY * PFNGLGETQUERYOBJECTIVPROC) (GLuint id, GLenum pname, GLint* params);
    typedef void      (GLAPIENTRY * PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint* params);
    typedef void      (GLAPIENTRY * PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint* params);
    typedef GLboolean (GLAPIENTRY * PFNGLISBUFFERPROC) (GLuint buffer);
    typedef GLboolean (GLAPIENTRY * PFNGLISQUERYPROC) (GLuint id);
    typedef void*     (GLAPIENTRY * PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
    typedef GLboolean (GLAPIENTRY * PFNGLUNMAPBUFFERPROC) (GLenum target);

    #define glBeginQuery GLEGetCurrentFunction(glBeginQuery)
    #define glBindBuffer GLEGetCurrentFunction(glBindBuffer)
    #define glBufferData GLEGetCurrentFunction(glBufferData)
    #define glBufferSubData GLEGetCurrentFunction(glBufferSubData)
    #define glDeleteBuffers GLEGetCurrentFunction(glDeleteBuffers)
    #define glDeleteQueries GLEGetCurrentFunction(glDeleteQueries)
    #define glEndQuery GLEGetCurrentFunction(glEndQuery)
    #define glGenBuffers GLEGetCurrentFunction(glGenBuffers)
    #define glGenQueries GLEGetCurrentFunction(glGenQueries)
    #define glGetBufferParameteriv GLEGetCurrentFunction(glGetBufferParameteriv)
    #define glGetBufferPointerv GLEGetCurrentFunction(glGetBufferPointerv)
    #define glGetBufferSubData GLEGetCurrentFunction(glGetBufferSubData)
    #define glGetQueryObjectiv GLEGetCurrentFunction(glGetQueryObjectiv)
    #define glGetQueryObjectuiv GLEGetCurrentFunction(glGetQueryObjectuiv)
    #define glGetQueryiv GLEGetCurrentFunction(glGetQueryiv)
    #define glIsBuffer GLEGetCurrentFunction(glIsBuffer)
    #define glIsQuery GLEGetCurrentFunction(glIsQuery)
    #define glMapBuffer GLEGetCurrentFunction(glMapBuffer)
    #define glUnmapBuffer GLEGetCurrentFunction(glUnmapBuffer)

#endif // GL_VERSION_1_5




#ifndef GL_VERSION_2_0
    #define GL_VERSION_2_0 1

    #define GL_BLEND_EQUATION_RGB GL_BLEND_EQUATION
    #define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
    #define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
    #define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
    #define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
    #define GL_CURRENT_VERTEX_ATTRIB 0x8626
    #define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
    #define GL_VERTEX_PROGRAM_TWO_SIDE 0x8643
    #define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
    #define GL_STENCIL_BACK_FUNC 0x8800
    #define GL_STENCIL_BACK_FAIL 0x8801
    #define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
    #define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
    #define GL_MAX_DRAW_BUFFERS 0x8824
    #define GL_DRAW_BUFFER0 0x8825
    #define GL_DRAW_BUFFER1 0x8826
    #define GL_DRAW_BUFFER2 0x8827
    #define GL_DRAW_BUFFER3 0x8828
    #define GL_DRAW_BUFFER4 0x8829
    #define GL_DRAW_BUFFER5 0x882A
    #define GL_DRAW_BUFFER6 0x882B
    #define GL_DRAW_BUFFER7 0x882C
    #define GL_DRAW_BUFFER8 0x882D
    #define GL_DRAW_BUFFER9 0x882E
    #define GL_DRAW_BUFFER10 0x882F
    #define GL_DRAW_BUFFER11 0x8830
    #define GL_DRAW_BUFFER12 0x8831
    #define GL_DRAW_BUFFER13 0x8832
    #define GL_DRAW_BUFFER14 0x8833
    #define GL_DRAW_BUFFER15 0x8834
    #define GL_BLEND_EQUATION_ALPHA 0x883D
    #define GL_POINT_SPRITE 0x8861
    #define GL_COORD_REPLACE 0x8862
    #define GL_MAX_VERTEX_ATTRIBS 0x8869
    #define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
    #define GL_MAX_TEXTURE_COORDS 0x8871
    #define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
    #define GL_FRAGMENT_SHADER 0x8B30
    #define GL_VERTEX_SHADER 0x8B31
    #define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
    #define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
    #define GL_MAX_VARYING_FLOATS 0x8B4B
    #define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
    #define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
    #define GL_SHADER_TYPE 0x8B4F
    #define GL_FLOAT_VEC2 0x8B50
    #define GL_FLOAT_VEC3 0x8B51
    #define GL_FLOAT_VEC4 0x8B52
    #define GL_INT_VEC2 0x8B53
    #define GL_INT_VEC3 0x8B54
    #define GL_INT_VEC4 0x8B55
    #define GL_BOOL 0x8B56
    #define GL_BOOL_VEC2 0x8B57
    #define GL_BOOL_VEC3 0x8B58
    #define GL_BOOL_VEC4 0x8B59
    #define GL_FLOAT_MAT2 0x8B5A
    #define GL_FLOAT_MAT3 0x8B5B
    #define GL_FLOAT_MAT4 0x8B5C
    #define GL_SAMPLER_1D 0x8B5D
    #define GL_SAMPLER_2D 0x8B5E
    #define GL_SAMPLER_3D 0x8B5F
    #define GL_SAMPLER_CUBE 0x8B60
    #define GL_SAMPLER_1D_SHADOW 0x8B61
    #define GL_SAMPLER_2D_SHADOW 0x8B62
    #define GL_DELETE_STATUS 0x8B80
    #define GL_COMPILE_STATUS 0x8B81
    #define GL_LINK_STATUS 0x8B82
    #define GL_VALIDATE_STATUS 0x8B83
    #define GL_INFO_LOG_LENGTH 0x8B84
    #define GL_ATTACHED_SHADERS 0x8B85
    #define GL_ACTIVE_UNIFORMS 0x8B86
    #define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
    #define GL_SHADER_SOURCE_LENGTH 0x8B88
    #define GL_ACTIVE_ATTRIBUTES 0x8B89
    #define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
    #define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
    #define GL_SHADING_LANGUAGE_VERSION 0x8B8C
    #define GL_CURRENT_PROGRAM 0x8B8D
    #define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
    #define GL_LOWER_LEFT 0x8CA1
    #define GL_UPPER_LEFT 0x8CA2
    #define GL_STENCIL_BACK_REF 0x8CA3
    #define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
    #define GL_STENCIL_BACK_WRITEMASK 0x8CA5

    typedef void (GLAPIENTRY * PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
    typedef void (GLAPIENTRY * PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar* name);
    typedef void (GLAPIENTRY * PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);
    typedef void (GLAPIENTRY * PFNGLCOMPILESHADERPROC) (GLuint shader);
    typedef GLuint (GLAPIENTRY * PFNGLCREATEPROGRAMPROC) (void);
    typedef GLuint (GLAPIENTRY * PFNGLCREATESHADERPROC) (GLenum type);
    typedef void (GLAPIENTRY * PFNGLDELETEPROGRAMPROC) (GLuint program);
    typedef void (GLAPIENTRY * PFNGLDELETESHADERPROC) (GLuint shader);
    typedef void (GLAPIENTRY * PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
    typedef void (GLAPIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
    typedef void (GLAPIENTRY * PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum* bufs);
    typedef void (GLAPIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
    typedef void (GLAPIENTRY * PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
    typedef void (GLAPIENTRY * PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
    typedef void (GLAPIENTRY * PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
    typedef GLint (GLAPIENTRY * PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar* name);
    typedef void (GLAPIENTRY * PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    typedef void (GLAPIENTRY * PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* param);
    typedef void (GLAPIENTRY * PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    typedef void (GLAPIENTRY * PFNGLGETSHADERSOURCEPROC) (GLuint obj, GLsizei maxLength, GLsizei* length, GLchar* source);
    typedef void (GLAPIENTRY * PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint* param);
    typedef GLint     (GLAPIENTRY * PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
    typedef void      (GLAPIENTRY * PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat* params);
    typedef void      (GLAPIENTRY * PFNGLGETUNIFORMIVPROC) (GLuint program, GLint location, GLint* params);
    typedef void      (GLAPIENTRY * PFNGLGETVERTEXATTRIBPOINTERVPROC) (GLuint index, GLenum pname, void** pointer);
    typedef void      (GLAPIENTRY * PFNGLGETVERTEXATTRIBDVPROC) (GLuint index, GLenum pname, GLdouble* params);
    typedef void      (GLAPIENTRY * PFNGLGETVERTEXATTRIBFVPROC) (GLuint index, GLenum pname, GLfloat* params);
    typedef void      (GLAPIENTRY * PFNGLGETVERTEXATTRIBIVPROC) (GLuint index, GLenum pname, GLint* params);
    typedef GLboolean (GLAPIENTRY * PFNGLISPROGRAMPROC) (GLuint program);
    typedef GLboolean (GLAPIENTRY * PFNGLISSHADERPROC) (GLuint shader);
    typedef void      (GLAPIENTRY * PFNGLLINKPROGRAMPROC) (GLuint program);
    typedef void      (GLAPIENTRY * PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
    typedef void      (GLAPIENTRY * PFNGLSTENCILFUNCSEPARATEPROC) (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
    typedef void      (GLAPIENTRY * PFNGLSTENCILMASKSEPARATEPROC) (GLenum face, GLuint mask);
    typedef void      (GLAPIENTRY * PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
    typedef void      (GLAPIENTRY * PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
    typedef void      (GLAPIENTRY * PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat* value);
    typedef void      (GLAPIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
    typedef void (GLAPIENTRY * PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
    typedef void (GLAPIENTRY * PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
    typedef void (GLAPIENTRY * PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    typedef void (GLAPIENTRY * PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM3IPROC) (GLint location, GLint v0, GLint v1, GLint v2);
    typedef void (GLAPIENTRY * PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    typedef void (GLAPIENTRY * PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM4IPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    typedef void (GLAPIENTRY * PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLUSEPROGRAMPROC) (GLuint program);
    typedef void (GLAPIENTRY * PFNGLVALIDATEPROGRAMPROC) (GLuint program);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB1DPROC) (GLuint index, GLdouble x);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB1DVPROC) (GLuint index, const GLdouble* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB1FPROC) (GLuint index, GLfloat x);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB1FVPROC) (GLuint index, const GLfloat* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB1SPROC) (GLuint index, GLshort x);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB1SVPROC) (GLuint index, const GLshort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB2DPROC) (GLuint index, GLdouble x, GLdouble y);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB2DVPROC) (GLuint index, const GLdouble* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB2FPROC) (GLuint index, GLfloat x, GLfloat y);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB2FVPROC) (GLuint index, const GLfloat* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB2SPROC) (GLuint index, GLshort x, GLshort y);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB2SVPROC) (GLuint index, const GLshort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB3DVPROC) (GLuint index, const GLdouble* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB3FVPROC) (GLuint index, const GLfloat* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB3SPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB3SVPROC) (GLuint index, const GLshort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NBVPROC) (GLuint index, const GLbyte* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NIVPROC) (GLuint index, const GLint* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NSVPROC) (GLuint index, const GLshort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NUBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NUBVPROC) (GLuint index, const GLubyte* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NUIVPROC) (GLuint index, const GLuint* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4NUSVPROC) (GLuint index, const GLushort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4BVPROC) (GLuint index, const GLbyte* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4IVPROC) (GLuint index, const GLint* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4SPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4SVPROC) (GLuint index, const GLshort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4UBVPROC) (GLuint index, const GLubyte* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4UIVPROC) (GLuint index, const GLuint* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIB4USVPROC) (GLuint index, const GLushort* v);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

    #define glAttachShader GLEGetCurrentFunction(glAttachShader)
    #define glBindAttribLocation GLEGetCurrentFunction(glBindAttribLocation)
    #define glBlendEquationSeparate GLEGetCurrentFunction(glBlendEquationSeparate)
    #define glCompileShader GLEGetCurrentFunction(glCompileShader)
    #define glCreateProgram GLEGetCurrentFunction(glCreateProgram)
    #define glCreateShader GLEGetCurrentFunction(glCreateShader)
    #define glDeleteProgram GLEGetCurrentFunction(glDeleteProgram)
    #define glDeleteShader GLEGetCurrentFunction(glDeleteShader)
    #define glDetachShader GLEGetCurrentFunction(glDetachShader)
    #define glDisableVertexAttribArray GLEGetCurrentFunction(glDisableVertexAttribArray)
    #define glDrawBuffers GLEGetCurrentFunction(glDrawBuffers)
    #define glEnableVertexAttribArray GLEGetCurrentFunction(glEnableVertexAttribArray)
    #define glGetActiveAttrib GLEGetCurrentFunction(glGetActiveAttrib)
    #define glGetActiveUniform GLEGetCurrentFunction(glGetActiveUniform)
    #define glGetAttachedShaders GLEGetCurrentFunction(glGetAttachedShaders)
    #define glGetAttribLocation GLEGetCurrentFunction(glGetAttribLocation)
    #define glGetProgramInfoLog GLEGetCurrentFunction(glGetProgramInfoLog)
    #define glGetProgramiv GLEGetCurrentFunction(glGetProgramiv)
    #define glGetShaderInfoLog GLEGetCurrentFunction(glGetShaderInfoLog)
    #define glGetShaderSource GLEGetCurrentFunction(glGetShaderSource)
    #define glGetShaderiv GLEGetCurrentFunction(glGetShaderiv)
    #define glGetUniformLocation GLEGetCurrentFunction(glGetUniformLocation)
    #define glGetUniformfv GLEGetCurrentFunction(glGetUniformfv)
    #define glGetUniformiv GLEGetCurrentFunction(glGetUniformiv)
    #define glGetVertexAttribPointerv GLEGetCurrentFunction(glGetVertexAttribPointerv)
    #define glGetVertexAttribdv GLEGetCurrentFunction(glGetVertexAttribdv)
    #define glGetVertexAttribfv GLEGetCurrentFunction(glGetVertexAttribfv)
    #define glGetVertexAttribiv GLEGetCurrentFunction(glGetVertexAttribiv)
    #define glIsProgram GLEGetCurrentFunction(glIsProgram)
    #define glIsShader GLEGetCurrentFunction(glIsShader)
    #define glLinkProgram GLEGetCurrentFunction(glLinkProgram)
    #define glShaderSource GLEGetCurrentFunction(glShaderSource)
    #define glStencilFuncSeparate GLEGetCurrentFunction(glStencilFuncSeparate)
    #define glStencilMaskSeparate GLEGetCurrentFunction(glStencilMaskSeparate)
    #define glStencilOpSeparate GLEGetCurrentFunction(glStencilOpSeparate)
    #define glUniform1f GLEGetCurrentFunction(glUniform1f)
    #define glUniform1fv GLEGetCurrentFunction(glUniform1fv)
    #define glUniform1i GLEGetCurrentFunction(glUniform1i)
    #define glUniform1iv GLEGetCurrentFunction(glUniform1iv)
    #define glUniform2f GLEGetCurrentFunction(glUniform2f)
    #define glUniform2fv GLEGetCurrentFunction(glUniform2fv)
    #define glUniform2i GLEGetCurrentFunction(glUniform2i)
    #define glUniform2iv GLEGetCurrentFunction(glUniform2iv)
    #define glUniform3f GLEGetCurrentFunction(glUniform3f)
    #define glUniform3fv GLEGetCurrentFunction(glUniform3fv)
    #define glUniform3i GLEGetCurrentFunction(glUniform3i)
    #define glUniform3iv GLEGetCurrentFunction(glUniform3iv)
    #define glUniform4f GLEGetCurrentFunction(glUniform4f)
    #define glUniform4fv GLEGetCurrentFunction(glUniform4fv)
    #define glUniform4i GLEGetCurrentFunction(glUniform4i)
    #define glUniform4iv GLEGetCurrentFunction(glUniform4iv)
    #define glUniformMatrix2fv GLEGetCurrentFunction(glUniformMatrix2fv)
    #define glUniformMatrix3fv GLEGetCurrentFunction(glUniformMatrix3fv)
    #define glUniformMatrix4fv GLEGetCurrentFunction(glUniformMatrix4fv)
    #define glUseProgram GLEGetCurrentFunction(glUseProgram)
    #define glValidateProgram GLEGetCurrentFunction(glValidateProgram)
    #define glVertexAttrib1d GLEGetCurrentFunction(glVertexAttrib1d)
    #define glVertexAttrib1dv GLEGetCurrentFunction(glVertexAttrib1dv)
    #define glVertexAttrib1f GLEGetCurrentFunction(glVertexAttrib1f)
    #define glVertexAttrib1fv GLEGetCurrentFunction(glVertexAttrib1fv)
    #define glVertexAttrib1s GLEGetCurrentFunction(glVertexAttrib1s)
    #define glVertexAttrib1sv GLEGetCurrentFunction(glVertexAttrib1sv)
    #define glVertexAttrib2d GLEGetCurrentFunction(glVertexAttrib2d)
    #define glVertexAttrib2dv GLEGetCurrentFunction(glVertexAttrib2dv)
    #define glVertexAttrib2f GLEGetCurrentFunction(glVertexAttrib2f)
    #define glVertexAttrib2fv GLEGetCurrentFunction(glVertexAttrib2fv)
    #define glVertexAttrib2s GLEGetCurrentFunction(glVertexAttrib2s)
    #define glVertexAttrib2sv GLEGetCurrentFunction(glVertexAttrib2sv)
    #define glVertexAttrib3d GLEGetCurrentFunction(glVertexAttrib3d)
    #define glVertexAttrib3dv GLEGetCurrentFunction(glVertexAttrib3dv)
    #define glVertexAttrib3f GLEGetCurrentFunction(glVertexAttrib3f)
    #define glVertexAttrib3fv GLEGetCurrentFunction(glVertexAttrib3fv)
    #define glVertexAttrib3s GLEGetCurrentFunction(glVertexAttrib3s)
    #define glVertexAttrib3sv GLEGetCurrentFunction(glVertexAttrib3sv)
    #define glVertexAttrib4Nbv GLEGetCurrentFunction(glVertexAttrib4Nbv)
    #define glVertexAttrib4Niv GLEGetCurrentFunction(glVertexAttrib4Niv)
    #define glVertexAttrib4Nsv GLEGetCurrentFunction(glVertexAttrib4Nsv)
    #define glVertexAttrib4Nub GLEGetCurrentFunction(glVertexAttrib4Nub)
    #define glVertexAttrib4Nubv GLEGetCurrentFunction(glVertexAttrib4Nubv)
    #define glVertexAttrib4Nuiv GLEGetCurrentFunction(glVertexAttrib4Nuiv)
    #define glVertexAttrib4Nusv GLEGetCurrentFunction(glVertexAttrib4Nusv)
    #define glVertexAttrib4bv GLEGetCurrentFunction(glVertexAttrib4bv)
    #define glVertexAttrib4d GLEGetCurrentFunction(glVertexAttrib4d)
    #define glVertexAttrib4dv GLEGetCurrentFunction(glVertexAttrib4dv)
    #define glVertexAttrib4f GLEGetCurrentFunction(glVertexAttrib4f)
    #define glVertexAttrib4fv GLEGetCurrentFunction(glVertexAttrib4fv)
    #define glVertexAttrib4iv GLEGetCurrentFunction(glVertexAttrib4iv)
    #define glVertexAttrib4s GLEGetCurrentFunction(glVertexAttrib4s)
    #define glVertexAttrib4sv GLEGetCurrentFunction(glVertexAttrib4sv)
    #define glVertexAttrib4ubv GLEGetCurrentFunction(glVertexAttrib4ubv)
    #define glVertexAttrib4uiv GLEGetCurrentFunction(glVertexAttrib4uiv)
    #define glVertexAttrib4usv GLEGetCurrentFunction(glVertexAttrib4usv)
    #define glVertexAttribPointer GLEGetCurrentFunction(glVertexAttribPointer)

#endif // GL_VERSION_2_0



#ifndef GL_VERSION_2_1
    #define GL_VERSION_2_1 1

    #define GL_CURRENT_RASTER_SECONDARY_COLOR 0x845F
    #define GL_PIXEL_PACK_BUFFER 0x88EB
    #define GL_PIXEL_UNPACK_BUFFER 0x88EC
    #define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
    #define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
    #define GL_FLOAT_MAT2x3 0x8B65
    #define GL_FLOAT_MAT2x4 0x8B66
    #define GL_FLOAT_MAT3x2 0x8B67
    #define GL_FLOAT_MAT3x4 0x8B68
    #define GL_FLOAT_MAT4x2 0x8B69
    #define GL_FLOAT_MAT4x3 0x8B6A
    #define GL_SRGB 0x8C40
    #define GL_SRGB8 0x8C41
    #define GL_SRGB_ALPHA 0x8C42
    #define GL_SRGB8_ALPHA8 0x8C43
    #define GL_SLUMINANCE_ALPHA 0x8C44
    #define GL_SLUMINANCE8_ALPHA8 0x8C45
    #define GL_SLUMINANCE 0x8C46
    #define GL_SLUMINANCE8 0x8C47
    #define GL_COMPRESSED_SRGB 0x8C48
    #define GL_COMPRESSED_SRGB_ALPHA 0x8C49
    #define GL_COMPRESSED_SLUMINANCE 0x8C4A
    #define GL_COMPRESSED_SLUMINANCE_ALPHA 0x8C4B

    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX2X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX2X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX3X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX3X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX4X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    typedef void (GLAPIENTRY * PFNGLUNIFORMMATRIX4X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

    #define glUniformMatrix2x3fv GLEGetCurrentFunction(glUniformMatrix2x3fv)
    #define glUniformMatrix2x4fv GLEGetCurrentFunction(glUniformMatrix2x4fv)
    #define glUniformMatrix3x2fv GLEGetCurrentFunction(glUniformMatrix3x2fv)
    #define glUniformMatrix3x4fv GLEGetCurrentFunction(glUniformMatrix3x4fv)
    #define glUniformMatrix4x2fv GLEGetCurrentFunction(glUniformMatrix4x2fv)
    #define glUniformMatrix4x3fv GLEGetCurrentFunction(glUniformMatrix4x3fv)

#endif // GL_VERSION_2_1




#ifndef GL_VERSION_3_0
    #define GL_VERSION_3_0 1

    #define GL_CLIP_DISTANCE0 GL_CLIP_PLANE0
    #define GL_CLIP_DISTANCE1 GL_CLIP_PLANE1
    #define GL_CLIP_DISTANCE2 GL_CLIP_PLANE2
    #define GL_CLIP_DISTANCE3 GL_CLIP_PLANE3
    #define GL_CLIP_DISTANCE4 GL_CLIP_PLANE4
    #define GL_CLIP_DISTANCE5 GL_CLIP_PLANE5
    #define GL_COMPARE_REF_TO_TEXTURE GL_COMPARE_R_TO_TEXTURE_ARB
    #define GL_MAX_CLIP_DISTANCES GL_MAX_CLIP_PLANES
    #define GL_MAX_VARYING_COMPONENTS GL_MAX_VARYING_FLOATS
    #define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x0001
    #define GL_MAJOR_VERSION 0x821B
    #define GL_MINOR_VERSION 0x821C
    #define GL_NUM_EXTENSIONS 0x821D
    #define GL_CONTEXT_FLAGS 0x821E
    #define GL_DEPTH_BUFFER 0x8223
    #define GL_STENCIL_BUFFER 0x8224
    #define GL_RGBA32F 0x8814
    #define GL_RGB32F 0x8815
    #define GL_RGBA16F 0x881A
    #define GL_RGB16F 0x881B
    #define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
    #define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
    #define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
    #define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
    #define GL_CLAMP_VERTEX_COLOR 0x891A
    #define GL_CLAMP_FRAGMENT_COLOR 0x891B
    #define GL_CLAMP_READ_COLOR 0x891C
    #define GL_FIXED_ONLY 0x891D
    #define GL_TEXTURE_RED_TYPE 0x8C10
    #define GL_TEXTURE_GREEN_TYPE 0x8C11
    #define GL_TEXTURE_BLUE_TYPE 0x8C12
    #define GL_TEXTURE_ALPHA_TYPE 0x8C13
    #define GL_TEXTURE_LUMINANCE_TYPE 0x8C14
    #define GL_TEXTURE_INTENSITY_TYPE 0x8C15
    #define GL_TEXTURE_DEPTH_TYPE 0x8C16
    #define GL_TEXTURE_1D_ARRAY 0x8C18
    #define GL_PROXY_TEXTURE_1D_ARRAY 0x8C19
    #define GL_TEXTURE_2D_ARRAY 0x8C1A
    #define GL_PROXY_TEXTURE_2D_ARRAY 0x8C1B
    #define GL_TEXTURE_BINDING_1D_ARRAY 0x8C1C
    #define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
    #define GL_R11F_G11F_B10F 0x8C3A
    #define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
    #define GL_RGB9_E5 0x8C3D
    #define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
    #define GL_TEXTURE_SHARED_SIZE 0x8C3F
    #define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
    #define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
    #define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
    #define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
    #define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
    #define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
    #define GL_PRIMITIVES_GENERATED 0x8C87
    #define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
    #define GL_RASTERIZER_DISCARD 0x8C89
    #define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
    #define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
    #define GL_INTERLEAVED_ATTRIBS 0x8C8C
    #define GL_SEPARATE_ATTRIBS 0x8C8D
    #define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
    #define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
    #define GL_RGBA32UI 0x8D70
    #define GL_RGB32UI 0x8D71
    #define GL_RGBA16UI 0x8D76
    #define GL_RGB16UI 0x8D77
    #define GL_RGBA8UI 0x8D7C
    #define GL_RGB8UI 0x8D7D
    #define GL_RGBA32I 0x8D82
    #define GL_RGB32I 0x8D83
    #define GL_RGBA16I 0x8D88
    #define GL_RGB16I 0x8D89
    #define GL_RGBA8I 0x8D8E
    #define GL_RGB8I 0x8D8F
    #define GL_RED_INTEGER 0x8D94
    #define GL_GREEN_INTEGER 0x8D95
    #define GL_BLUE_INTEGER 0x8D96
    #define GL_ALPHA_INTEGER 0x8D97
    #define GL_RGB_INTEGER 0x8D98
    #define GL_RGBA_INTEGER 0x8D99
    #define GL_BGR_INTEGER 0x8D9A
    #define GL_BGRA_INTEGER 0x8D9B
    #define GL_SAMPLER_1D_ARRAY 0x8DC0
    #define GL_SAMPLER_2D_ARRAY 0x8DC1
    #define GL_SAMPLER_1D_ARRAY_SHADOW 0x8DC3
    #define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
    #define GL_SAMPLER_CUBE_SHADOW 0x8DC5
    #define GL_UNSIGNED_INT_VEC2 0x8DC6
    #define GL_UNSIGNED_INT_VEC3 0x8DC7
    #define GL_UNSIGNED_INT_VEC4 0x8DC8
    #define GL_INT_SAMPLER_1D 0x8DC9
    #define GL_INT_SAMPLER_2D 0x8DCA
    #define GL_INT_SAMPLER_3D 0x8DCB
    #define GL_INT_SAMPLER_CUBE 0x8DCC
    #define GL_INT_SAMPLER_1D_ARRAY 0x8DCE
    #define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
    #define GL_UNSIGNED_INT_SAMPLER_1D 0x8DD1
    #define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
    #define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
    #define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
    #define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 0x8DD6
    #define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
    #define GL_QUERY_WAIT 0x8E13
    #define GL_QUERY_NO_WAIT 0x8E14
    #define GL_QUERY_BY_REGION_WAIT 0x8E15
    #define GL_QUERY_BY_REGION_NO_WAIT 0x8E16

    typedef void (GLAPIENTRY * PFNGLBEGINCONDITIONALRENDERPROC) (GLuint id, GLenum mode);
    typedef void (GLAPIENTRY * PFNGLBEGINTRANSFORMFEEDBACKPROC) (GLenum primitiveMode);
    typedef void (GLAPIENTRY * PFNGLBINDFRAGDATALOCATIONPROC) (GLuint program, GLuint colorNumber, const GLchar* name);
    typedef void (GLAPIENTRY * PFNGLCLAMPCOLORPROC) (GLenum target, GLenum clamp);
    typedef void (GLAPIENTRY * PFNGLCLEARBUFFERFIPROC) (GLenum buffer, GLint drawBuffer, GLfloat depth, GLint stencil);
    typedef void (GLAPIENTRY * PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawBuffer, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLCLEARBUFFERIVPROC) (GLenum buffer, GLint drawBuffer, const GLint* value);
    typedef void (GLAPIENTRY * PFNGLCLEARBUFFERUIVPROC) (GLenum buffer, GLint drawBuffer, const GLuint* value);
    typedef void (GLAPIENTRY * PFNGLCOLORMASKIPROC) (GLuint buf, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    typedef void (GLAPIENTRY * PFNGLDISABLEIPROC) (GLenum cap, GLuint index);
    typedef void (GLAPIENTRY * PFNGLENABLEIPROC) (GLenum cap, GLuint index);
    typedef void (GLAPIENTRY * PFNGLENDCONDITIONALRENDERPROC) (void);
    typedef void (GLAPIENTRY * PFNGLENDTRANSFORMFEEDBACKPROC) (void);
    typedef void (GLAPIENTRY * PFNGLBINDBUFFERRANGEPROC) (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
    typedef void (GLAPIENTRY * PFNGLBINDBUFFERBASEPROC) (GLenum target, GLuint index, GLuint buffer);
    typedef void (GLAPIENTRY * PFNGLGETBOOLEANI_VPROC) (GLenum pname, GLuint index, GLboolean* data);
    typedef void (GLAPIENTRY * PFNGLGETINTEGERI_VPROC) (GLenum target, GLuint index, GLint* data);
    typedef GLint (GLAPIENTRY * PFNGLGETFRAGDATALOCATIONPROC) (GLuint program, const GLchar* name);
    typedef const GLubyte* (GLAPIENTRY * PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
    typedef void (GLAPIENTRY * PFNGLGETTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, GLuint* params);
    typedef void (GLAPIENTRY * PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name);
    typedef void (GLAPIENTRY * PFNGLGETUNIFORMUIVPROC) (GLuint program, GLint location, GLuint* params);
    typedef void (GLAPIENTRY * PFNGLGETVERTEXATTRIBIIVPROC) (GLuint index, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETVERTEXATTRIBIUIVPROC) (GLuint index, GLenum pname, GLuint* params);
    typedef GLboolean (GLAPIENTRY * PFNGLISENABLEDIPROC) (GLenum cap, GLuint index);
    typedef void (GLAPIENTRY * PFNGLTEXPARAMETERIIVPROC) (GLenum target, GLenum pname, const GLint* params);
    typedef void (GLAPIENTRY * PFNGLTEXPARAMETERIUIVPROC) (GLenum target, GLenum pname, const GLuint* params);
    typedef void (GLAPIENTRY * PFNGLTRANSFORMFEEDBACKVARYINGSPROC) (GLuint program, GLsizei count, const GLchar *const* varyings, GLenum bufferMode);
    typedef void (GLAPIENTRY * PFNGLUNIFORM1UIPROC) (GLint location, GLuint v0);
    typedef void (GLAPIENTRY * PFNGLUNIFORM1UIVPROC) (GLint location, GLsizei count, const GLuint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM2UIPROC) (GLint location, GLuint v0, GLuint v1);
    typedef void (GLAPIENTRY * PFNGLUNIFORM2UIVPROC) (GLint location, GLsizei count, const GLuint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM3UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2);
    typedef void (GLAPIENTRY * PFNGLUNIFORM3UIVPROC) (GLint location, GLsizei count, const GLuint* value);
    typedef void (GLAPIENTRY * PFNGLUNIFORM4UIPROC) (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
    typedef void (GLAPIENTRY * PFNGLUNIFORM4UIVPROC) (GLint location, GLsizei count, const GLuint* value);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI1IPROC) (GLuint index, GLint v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI1IVPROC) (GLuint index, const GLint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI1UIPROC) (GLuint index, GLuint v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI1UIVPROC) (GLuint index, const GLuint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI2IPROC) (GLuint index, GLint v0, GLint v1);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI2IVPROC) (GLuint index, const GLint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI2UIPROC) (GLuint index, GLuint v0, GLuint v1);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI2UIVPROC) (GLuint index, const GLuint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI3IPROC) (GLuint index, GLint v0, GLint v1, GLint v2);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI3IVPROC) (GLuint index, const GLint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI3UIPROC) (GLuint index, GLuint v0, GLuint v1, GLuint v2);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI3UIVPROC) (GLuint index, const GLuint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4BVPROC) (GLuint index, const GLbyte* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4IPROC) (GLuint index, GLint v0, GLint v1, GLint v2, GLint v3);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4IVPROC) (GLuint index, const GLint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4SVPROC) (GLuint index, const GLshort* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4UBVPROC) (GLuint index, const GLubyte* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4UIPROC) (GLuint index, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4UIVPROC) (GLuint index, const GLuint* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBI4USVPROC) (GLuint index, const GLushort* v0);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBIPOINTERPROC) (GLuint index, GLint size, GLenum type, GLsizei stride, const void*pointer);

    #define glBeginConditionalRender GLEGetCurrentFunction(glBeginConditionalRender)
    #define glBeginTransformFeedback GLEGetCurrentFunction(glBeginTransformFeedback)
    #define glBindFragDataLocation GLEGetCurrentFunction(glBindFragDataLocation)
    #define glClampColor GLEGetCurrentFunction(glClampColor)
    #define glClearBufferfi GLEGetCurrentFunction(glClearBufferfi)
    #define glClearBufferfv GLEGetCurrentFunction(glClearBufferfv)
    #define glClearBufferiv GLEGetCurrentFunction(glClearBufferiv)
    #define glClearBufferuiv GLEGetCurrentFunction(glClearBufferuiv)
    #define glColorMaski GLEGetCurrentFunction(glColorMaski)
    #define glDisablei GLEGetCurrentFunction(glDisablei)
    #define glEnablei GLEGetCurrentFunction(glEnablei)
    #define glEndConditionalRender GLEGetCurrentFunction(glEndConditionalRender)
    #define glEndTransformFeedback GLEGetCurrentFunction(glEndTransformFeedback)
    #define glGetBooleani_v GLEGetCurrentFunction(glGetBooleani_v)
    #define glGetIntegeri_v GLEGetCurrentFunction(glGetIntegeri_v)
    #define glGetFragDataLocation GLEGetCurrentFunction(glGetFragDataLocation)
    #define glGetStringi GLEGetCurrentFunction(glGetStringi)
    #define glGetTexParameterIiv GLEGetCurrentFunction(glGetTexParameterIiv)
    #define glGetTexParameterIuiv GLEGetCurrentFunction(glGetTexParameterIuiv)
    #define glGetTransformFeedbackVarying GLEGetCurrentFunction(glGetTransformFeedbackVarying)
    #define glGetUniformuiv GLEGetCurrentFunction(glGetUniformuiv)
    #define glGetVertexAttribIiv GLEGetCurrentFunction(glGetVertexAttribIiv)
    #define glGetVertexAttribIuiv GLEGetCurrentFunction(glGetVertexAttribIuiv)
    #define glIsEnabledi GLEGetCurrentFunction(glIsEnabledi)
    #define glTexParameterIiv GLEGetCurrentFunction(glTexParameterIiv)
    #define glTexParameterIuiv GLEGetCurrentFunction(glTexParameterIuiv)
    #define glTransformFeedbackVaryings GLEGetCurrentFunction(glTransformFeedbackVaryings)
    #define glUniform1ui GLEGetCurrentFunction(glUniform1ui)
    #define glUniform1uiv GLEGetCurrentFunction(glUniform1uiv)
    #define glUniform2ui GLEGetCurrentFunction(glUniform2ui)
    #define glUniform2uiv GLEGetCurrentFunction(glUniform2uiv)
    #define glUniform3ui GLEGetCurrentFunction(glUniform3ui)
    #define glUniform3uiv GLEGetCurrentFunction(glUniform3uiv)
    #define glUniform4ui GLEGetCurrentFunction(glUniform4ui)
    #define glUniform4uiv GLEGetCurrentFunction(glUniform4uiv)
    #define glVertexAttribI1i GLEGetCurrentFunction(glVertexAttribI1i)
    #define glVertexAttribI1iv GLEGetCurrentFunction(glVertexAttribI1iv)
    #define glVertexAttribI1ui GLEGetCurrentFunction(glVertexAttribI1ui)
    #define glVertexAttribI1uiv GLEGetCurrentFunction(glVertexAttribI1uiv)
    #define glVertexAttribI2i GLEGetCurrentFunction(glVertexAttribI2i)
    #define glVertexAttribI2iv GLEGetCurrentFunction(glVertexAttribI2iv)
    #define glVertexAttribI2ui GLEGetCurrentFunction(glVertexAttribI2ui)
    #define glVertexAttribI2uiv GLEGetCurrentFunction(glVertexAttribI2uiv)
    #define glVertexAttribI3i GLEGetCurrentFunction(glVertexAttribI3i)
    #define glVertexAttribI3iv GLEGetCurrentFunction(glVertexAttribI3iv)
    #define glVertexAttribI3ui GLEGetCurrentFunction(glVertexAttribI3ui)
    #define glVertexAttribI3uiv GLEGetCurrentFunction(glVertexAttribI3uiv)
    #define glVertexAttribI4bv GLEGetCurrentFunction(glVertexAttribI4bv)
    #define glVertexAttribI4i GLEGetCurrentFunction(glVertexAttribI4i)
    #define glVertexAttribI4iv GLEGetCurrentFunction(glVertexAttribI4iv)
    #define glVertexAttribI4sv GLEGetCurrentFunction(glVertexAttribI4sv)
    #define glVertexAttribI4ubv GLEGetCurrentFunction(glVertexAttribI4ubv)
    #define glVertexAttribI4ui GLEGetCurrentFunction(glVertexAttribI4ui)
    #define glVertexAttribI4uiv GLEGetCurrentFunction(glVertexAttribI4uiv)
    #define glVertexAttribI4usv GLEGetCurrentFunction(glVertexAttribI4usv)
    #define glVertexAttribIPointer GLEGetCurrentFunction(glVertexAttribIPointer)

#endif // GL_VERSION_3_0




#ifndef GL_VERSION_3_1
    #define GL_VERSION_3_1 1

    #define GL_TEXTURE_RECTANGLE 0x84F5
    #define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
    #define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
    #define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
    #define GL_SAMPLER_2D_RECT 0x8B63
    #define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
    #define GL_TEXTURE_BUFFER 0x8C2A
    #define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
    #define GL_TEXTURE_BINDING_BUFFER 0x8C2C
    #define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
    #define GL_TEXTURE_BUFFER_FORMAT 0x8C2E
    #define GL_SAMPLER_BUFFER 0x8DC2
    #define GL_INT_SAMPLER_2D_RECT 0x8DCD
    #define GL_INT_SAMPLER_BUFFER 0x8DD0
    #define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
    #define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8
    #define GL_RED_SNORM 0x8F90
    #define GL_RG_SNORM 0x8F91
    #define GL_RGB_SNORM 0x8F92
    #define GL_RGBA_SNORM 0x8F93
    #define GL_R8_SNORM 0x8F94
    #define GL_RG8_SNORM 0x8F95
    #define GL_RGB8_SNORM 0x8F96
    #define GL_RGBA8_SNORM 0x8F97
    #define GL_R16_SNORM 0x8F98
    #define GL_RG16_SNORM 0x8F99
    #define GL_RGB16_SNORM 0x8F9A
    #define GL_RGBA16_SNORM 0x8F9B
    #define GL_SIGNED_NORMALIZED 0x8F9C
    #define GL_PRIMITIVE_RESTART 0x8F9D
    #define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
    #define GL_BUFFER_ACCESS_FLAGS 0x911F
    #define GL_BUFFER_MAP_LENGTH 0x9120
    #define GL_BUFFER_MAP_OFFSET 0x9121

    typedef void (GLAPIENTRY * PFNGLDRAWARRAYSINSTANCEDPROC) (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
    typedef void (GLAPIENTRY * PFNGLDRAWELEMENTSINSTANCEDPROC) (GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount);
    typedef void (GLAPIENTRY * PFNGLPRIMITIVERESTARTINDEXPROC) (GLuint buffer);
    typedef void (GLAPIENTRY * PFNGLTEXBUFFERPROC) (GLenum target, GLenum internalFormat, GLuint buffer);

    #define glDrawArraysInstanced GLEGetCurrentFunction(glDrawArraysInstanced)
    #define glDrawElementsInstanced GLEGetCurrentFunction(glDrawElementsInstanced)
    #define glPrimitiveRestartIndex GLEGetCurrentFunction(glPrimitiveRestartIndex)
    #define glTexBuffer GLEGetCurrentFunction(glTexBuffer)

    #define GLE_VERSION_3_1 GLEGetCurrentVariable(gl_VERSION_3_1)

#endif // GL_VERSION_3_1



#ifndef GL_VERSION_3_2
    #define GL_VERSION_3_2 1

    #define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
    #define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
    #define GL_LINES_ADJACENCY 0x000A
    #define GL_LINE_STRIP_ADJACENCY 0x000B
    #define GL_TRIANGLES_ADJACENCY 0x000C
    #define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
    #define GL_PROGRAM_POINT_SIZE 0x8642
    #define GL_GEOMETRY_VERTICES_OUT 0x8916
    #define GL_GEOMETRY_INPUT_TYPE 0x8917
    #define GL_GEOMETRY_OUTPUT_TYPE 0x8918
    #define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
    #define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
    #define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
    #define GL_GEOMETRY_SHADER 0x8DD9
    #define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
    #define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
    #define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
    #define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
    #define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
    #define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
    #define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
    #define GL_CONTEXT_PROFILE_MASK 0x9126

    typedef void (GLAPIENTRY * PFNGLFRAMEBUFFERTEXTUREPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
    typedef void (GLAPIENTRY * PFNGLGETBUFFERPARAMETERI64VPROC) (GLenum target, GLenum value, GLint64 * data);
    typedef void (GLAPIENTRY * PFNGLGETINTEGER64I_VPROC) (GLenum pname, GLuint index, GLint64 * data);

    #define glFramebufferTexture GLEGetCurrentFunction(glFramebufferTexture)
    #define glGetBufferParameteri64v GLEGetCurrentFunction(glGetBufferParameteri64v)
    #define glGetInteger64i_v GLEGetCurrentFunction(glGetInteger64i_v)

    #define GLE_VERSION_3_2 GLEGetCurrentVariable(gl_VERSION_3_2)
    
#endif // GL_VERSION_3_2



#ifndef GL_VERSION_3_3
    #define GL_VERSION_3_3 1

    #define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
    #define GL_RGB10_A2UI 0x906F

    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBDIVISORPROC) (GLuint index, GLuint divisor);

    #define glVertexAttribDivisor GLEGetCurrentFunction(glVertexAttribDivisor)
#endif



#ifndef GL_VERSION_4_0
    #define GL_VERSION_4_0 1

    #define GL_SAMPLE_SHADING 0x8C36
    #define GL_MIN_SAMPLE_SHADING_VALUE 0x8C37
    #define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
    #define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
    #define GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS 0x8F9F
    #define GL_TEXTURE_CUBE_MAP_ARRAY 0x9009
    #define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
    #define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY 0x900B
    #define GL_SAMPLER_CUBE_MAP_ARRAY 0x900C
    #define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW 0x900D
    #define GL_INT_SAMPLER_CUBE_MAP_ARRAY 0x900E
    #define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F

    typedef void (GLAPIENTRY * PFNGLBLENDEQUATIONSEPARATEIPROC) (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
    typedef void (GLAPIENTRY * PFNGLBLENDEQUATIONIPROC) (GLuint buf, GLenum mode);
    typedef void (GLAPIENTRY * PFNGLBLENDFUNCSEPARATEIPROC) (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    typedef void (GLAPIENTRY * PFNGLBLENDFUNCIPROC) (GLuint buf, GLenum src, GLenum dst);
    typedef void (GLAPIENTRY * PFNGLMINSAMPLESHADINGPROC) (GLclampf value);

    #define glBlendEquationSeparatei GLEGetCurrentFunction(glBlendEquationSeparatei)
    #define glBlendEquationi GLEGetCurrentFunction(glBlendEquationi)
    #define glBlendFuncSeparatei GLEGetCurrentFunction(glBlendFuncSeparatei)
    #define glBlendFunci GLEGetCurrentFunction(glBlendFunci)
    #define glMinSampleShading GLEGetCurrentFunction(glMinSampleShading)

#endif // GL_VERSION_4_0




#ifndef GL_VERSION_4_1
    #define GL_VERSION_4_1 1
    // Empty
#endif



#ifndef GL_VERSION_4_2
    #define GL_VERSION_4_2 1

    #define GL_COMPRESSED_RGBA_BPTC_UNORM 0x8E8C
    #define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
    #define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
    #define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F
#endif



#ifndef GL_VERSION_4_3
    #define GL_VERSION_4_3 1
    
    #define GL_NUM_SHADING_LANGUAGE_VERSIONS 0x82E9
    #define GL_VERTEX_ATTRIB_ARRAY_LONG 0x874E
#endif



#ifndef GL_VERSION_4_4
    #define GL_VERSION_4_4 1
    
    #define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
    #define GL_MAX_VERTEX_ATTRIB_STRIDE 0x82E5
    #define GL_TEXTURE_BUFFER_BINDING 0x8C2A
#endif



#ifndef GL_VERSION_4_5
    #define GL_VERSION_4_5 1
    // Empty
#endif



#ifndef GL_AMD_debug_output
    #define GL_AMD_debug_output 1

    #define GL_MAX_DEBUG_MESSAGE_LENGTH_AMD 0x9143
    #define GL_MAX_DEBUG_LOGGED_MESSAGES_AMD 0x9144
    #define GL_DEBUG_LOGGED_MESSAGES_AMD 0x9145
    #define GL_DEBUG_SEVERITY_HIGH_AMD 0x9146
    #define GL_DEBUG_SEVERITY_MEDIUM_AMD 0x9147
    #define GL_DEBUG_SEVERITY_LOW_AMD 0x9148
    #define GL_DEBUG_CATEGORY_API_ERROR_AMD 0x9149
    #define GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD 0x914A
    #define GL_DEBUG_CATEGORY_DEPRECATION_AMD 0x914B
    #define GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD 0x914C
    #define GL_DEBUG_CATEGORY_PERFORMANCE_AMD 0x914D
    #define GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD 0x914E
    #define GL_DEBUG_CATEGORY_APPLICATION_AMD 0x914F
    #define GL_DEBUG_CATEGORY_OTHER_AMD 0x9150

    typedef void (GLAPIENTRY *GLDEBUGPROCAMD)(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, void* userParam);

    typedef void   (GLAPIENTRY * PFNGLDEBUGMESSAGECALLBACKAMDPROC) (GLDEBUGPROCAMD callback, void *userParam);
    typedef void   (GLAPIENTRY * PFNGLDEBUGMESSAGEENABLEAMDPROC) (GLenum category, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
    typedef void   (GLAPIENTRY * PFNGLDEBUGMESSAGEINSERTAMDPROC) (GLenum category, GLenum severity, GLuint id, GLsizei length, const GLchar* buf);
    typedef GLuint (GLAPIENTRY * PFNGLGETDEBUGMESSAGELOGAMDPROC) (GLuint count, GLsizei bufsize, GLenum* categories, GLuint* severities, GLuint* ids, GLsizei* lengths, GLchar* message);

    #define glDebugMessageCallbackAMD GLEGetCurrentFunction(glDebugMessageCallbackAMD)
    #define glDebugMessageEnableAMD   GLEGetCurrentFunction(glDebugMessageEnableAMD)
    #define glDebugMessageInsertAMD   GLEGetCurrentFunction(glDebugMessageInsertAMD)
    #define glGetDebugMessageLogAMD   GLEGetCurrentFunction(glGetDebugMessageLogAMD)

    #define GLE_AMD_debug_output GLEGetCurrentVariable(gl_AMD_debug_output)

#endif // GL_AMD_debug_output



/* Disabled until needed
#ifndef GL_AMD_performance_monitor
    #define GL_AMD_performance_monitor 1

    #define GL_COUNTER_TYPE_AMD 0x8BC0
    #define GL_COUNTER_RANGE_AMD 0x8BC1
    #define GL_UNSIGNED_INT64_AMD 0x8BC2
    #define GL_PERCENTAGE_AMD 0x8BC3
    #define GL_PERFMON_RESULT_AVAILABLE_AMD 0x8BC4
    #define GL_PERFMON_RESULT_SIZE_AMD 0x8BC5
    #define GL_PERFMON_RESULT_AMD 0x8BC6

    typedef void (GLAPIENTRY * PFNGLBEGINPERFMONITORAMDPROC) (GLuint monitor);
    typedef void (GLAPIENTRY * PFNGLDELETEPERFMONITORSAMDPROC) (GLsizei n, GLuint* monitors);
    typedef void (GLAPIENTRY * PFNGLENDPERFMONITORAMDPROC) (GLuint monitor);
    typedef void (GLAPIENTRY * PFNGLGENPERFMONITORSAMDPROC) (GLsizei n, GLuint* monitors);
    typedef void (GLAPIENTRY * PFNGLGETPERFMONITORCOUNTERDATAAMDPROC) (GLuint monitor, GLenum pname, GLsizei dataSize, GLuint* data, GLint *bytesWritten);
    typedef void (GLAPIENTRY * PFNGLGETPERFMONITORCOUNTERINFOAMDPROC) (GLuint group, GLuint counter, GLenum pname, void *data);
    typedef void (GLAPIENTRY * PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC) (GLuint group, GLuint counter, GLsizei bufSize, GLsizei* length, GLchar *counterString);
    typedef void (GLAPIENTRY * PFNGLGETPERFMONITORCOUNTERSAMDPROC) (GLuint group, GLint* numCounters, GLint *maxActiveCounters, GLsizei countersSize, GLuint *counters);
    typedef void (GLAPIENTRY * PFNGLGETPERFMONITORGROUPSTRINGAMDPROC) (GLuint group, GLsizei bufSize, GLsizei* length, GLchar *groupString);
    typedef void (GLAPIENTRY * PFNGLGETPERFMONITORGROUPSAMDPROC) (GLint* numGroups, GLsizei groupsSize, GLuint *groups);
    typedef void (GLAPIENTRY * PFNGLSELECTPERFMONITORCOUNTERSAMDPROC) (GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint* counterList);

    #define glBeginPerfMonitorAMD GLEGetCurrentFunction(glBeginPerfMonitorAMD)
    #define glDeletePerfMonitorsAMD GLEGetCurrentFunction(glDeletePerfMonitorsAMD)
    #define glEndPerfMonitorAMD GLEGetCurrentFunction(glEndPerfMonitorAMD)
    #define glGenPerfMonitorsAMD GLEGetCurrentFunction(glGenPerfMonitorsAMD)
    #define glGetPerfMonitorCounterDataAMD GLEGetCurrentFunction(glGetPerfMonitorCounterDataAMD)
    #define glGetPerfMonitorCounterInfoAMD GLEGetCurrentFunction(glGetPerfMonitorCounterInfoAMD)
    #define glGetPerfMonitorCounterStringAMD GLEGetCurrentFunction(glGetPerfMonitorCounterStringAMD)
    #define glGetPerfMonitorCountersAMD GLEGetCurrentFunction(glGetPerfMonitorCountersAMD)
    #define glGetPerfMonitorGroupStringAMD GLEGetCurrentFunction(glGetPerfMonitorGroupStringAMD)
    #define glGetPerfMonitorGroupsAMD GLEGetCurrentFunction(glGetPerfMonitorGroupsAMD)
    #define glSelectPerfMonitorCountersAMD GLEGetCurrentFunction(glSelectPerfMonitorCountersAMD)

    #define GLE_AMD_performance_monitor GLEGetCurrentVariable(gl_AMD_performance_monitor)

#endif // GL_AMD_performance_monitor
*/


#if defined(GLE_APPLE_ENABLED)
    #ifndef GL_APPLE_aux_depth_stencil
        #define GL_APPLE_aux_depth_stencil 1

        #define GL_AUX_DEPTH_STENCIL_APPLE 0x8A14g

        #define GLE_APPLE_aux_depth_stencil GLEGetCurrentVariable(gl_APPLE_aux_depth_stencil)
    #endif



    #ifndef GL_APPLE_client_storage
        #define GL_APPLE_client_storage 1

        #define GL_UNPACK_CLIENT_STORAGE_APPLE 0x85B2

        #define GLE_APPLE_client_storage GLEGetCurrentVariable(gl_APPLE_client_storage)
    #endif



    #ifndef GL_APPLE_element_array
        #define GL_APPLE_element_array 1

        #define GL_ELEMENT_ARRAY_APPLE 0x8A0C
        #define GL_ELEMENT_ARRAY_TYPE_APPLE 0x8A0D
        #define GL_ELEMENT_ARRAY_POINTER_APPLE 0x8A0E

        typedef void (GLAPIENTRY * PFNGLDRAWELEMENTARRAYAPPLEPROC) (GLenum mode, GLint first, GLsizei count);
        typedef void (GLAPIENTRY * PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC) (GLenum mode, GLuint start, GLuint end, GLint first, GLsizei count);
        typedef void (GLAPIENTRY * PFNGLELEMENTPOINTERAPPLEPROC) (GLenum type, const void *pointer);
        typedef void (GLAPIENTRY * PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC) (GLenum mode, const GLint* first, const GLsizei *count, GLsizei primcount);
        typedef void (GLAPIENTRY * PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC) (GLenum mode, GLuint start, GLuint end, const GLint* first, const GLsizei *count, GLsizei primcount);

        #define glDrawElementArrayAPPLE GLEGetCurrentFunction(glDrawElementArrayAPPLE)
        #define glDrawRangeElementArrayAPPLE GLEGetCurrentFunction(glDrawRangeElementArrayAPPLE)
        #define glElementPointerAPPLE GLEGetCurrentFunction(glElementPointerAPPLE)
        #define glMultiDrawElementArrayAPPLE GLEGetCurrentFunction(glMultiDrawElementArrayAPPLE)
        #define glMultiDrawRangeElementArrayAPPLE GLEGetCurrentFunction(glMultiDrawRangeElementArrayAPPLE)

        #define GLE_APPLE_element_array GLEGetCurrentVariable(gl_APPLE_element_array)
    #endif



    #ifndef GL_APPLE_fence
        #define GL_APPLE_fence 1

        #define GL_DRAW_PIXELS_APPLE 0x8A0A
        #define GL_FENCE_APPLE 0x8A0B

        typedef void (GLAPIENTRY * PFNGLDELETEFENCESAPPLEPROC) (GLsizei n, const GLuint* fences);
        typedef void (GLAPIENTRY * PFNGLFINISHFENCEAPPLEPROC) (GLuint fence);
        typedef void (GLAPIENTRY * PFNGLFINISHOBJECTAPPLEPROC) (GLenum object, GLint name);
        typedef void (GLAPIENTRY * PFNGLGENFENCESAPPLEPROC) (GLsizei n, GLuint* fences);
        typedef GLboolean (GLAPIENTRY * PFNGLISFENCEAPPLEPROC) (GLuint fence);
        typedef void (GLAPIENTRY * PFNGLSETFENCEAPPLEPROC) (GLuint fence);
        typedef GLboolean (GLAPIENTRY * PFNGLTESTFENCEAPPLEPROC) (GLuint fence);
        typedef GLboolean (GLAPIENTRY * PFNGLTESTOBJECTAPPLEPROC) (GLenum object, GLuint name);

        #define glDeleteFencesAPPLE GLEGetCurrentFunction(glDeleteFencesAPPLE)
        #define glFinishFenceAPPLE GLEGetCurrentFunction(glFinishFenceAPPLE)
        #define glFinishObjectAPPLE GLEGetCurrentFunction(glFinishObjectAPPLE)
        #define glGenFencesAPPLE GLEGetCurrentFunction(glGenFencesAPPLE)
        #define glIsFenceAPPLE GLEGetCurrentFunction(glIsFenceAPPLE)
        #define glSetFenceAPPLE GLEGetCurrentFunction(glSetFenceAPPLE)
        #define glTestFenceAPPLE GLEGetCurrentFunction(glTestFenceAPPLE)
        #define glTestObjectAPPLE GLEGetCurrentFunction(glTestObjectAPPLE)

        #define GLE_APPLE_fence GLEGetCurrentVariable(gl_APPLE_fence)

    #endif



    #ifndef GL_APPLE_float_pixels
        #define GL_APPLE_float_pixels 1

        #define GL_HALF_APPLE 0x140B
        #define GL_RGBA_FLOAT32_APPLE 0x8814
        #define GL_RGB_FLOAT32_APPLE 0x8815
        #define GL_ALPHA_FLOAT32_APPLE 0x8816
        #define GL_INTENSITY_FLOAT32_APPLE 0x8817
        #define GL_LUMINANCE_FLOAT32_APPLE 0x8818
        #define GL_LUMINANCE_ALPHA_FLOAT32_APPLE 0x8819
        #define GL_RGBA_FLOAT16_APPLE 0x881A
        #define GL_RGB_FLOAT16_APPLE 0x881B
        #define GL_ALPHA_FLOAT16_APPLE 0x881C
        #define GL_INTENSITY_FLOAT16_APPLE 0x881D
        #define GL_LUMINANCE_FLOAT16_APPLE 0x881E
        #define GL_LUMINANCE_ALPHA_FLOAT16_APPLE 0x881F
        #define GL_COLOR_FLOAT_APPLE 0x8A0F

        #define GLE_APPLE_float_pixels GLEGetCurrentVariable(gl_APPLE_float_pixels)
    #endif



    #ifndef GL_APPLE_flush_buffer_range
        #define GL_APPLE_flush_buffer_range 1

        #define GL_BUFFER_SERIALIZED_MODIFY_APPLE 0x8A12
        #define GL_BUFFER_FLUSHING_UNMAP_APPLE 0x8A13

        typedef void (GLAPIENTRY * PFNGLBUFFERPARAMETERIAPPLEPROC) (GLenum target, GLenum pname, GLint param);
        typedef void (GLAPIENTRY * PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC) (GLenum target, GLintptr offset, GLsizeiptr size);

        #define glBufferParameteriAPPLE GLEGetCurrentFunction(glBufferParameteriAPPLE)
        #define glFlushMappedBufferRangeAPPLE GLEGetCurrentFunction(glFlushMappedBufferRangeAPPLE)

        #define GLE_APPLE_flush_buffer_range GLEGetCurrentVariable(gl_APPLE_flush_buffer_range)
    #endif



    #ifndef GL_APPLE_object_purgeable
        #define GL_APPLE_object_purgeable 1

        #define GL_BUFFER_OBJECT_APPLE 0x85B3
        #define GL_RELEASED_APPLE 0x8A19
        #define GL_VOLATILE_APPLE 0x8A1A
        #define GL_RETAINED_APPLE 0x8A1B
        #define GL_UNDEFINED_APPLE 0x8A1C
        #define GL_PURGEABLE_APPLE 0x8A1D

        typedef void (GLAPIENTRY * PFNGLGETOBJECTPARAMETERIVAPPLEPROC) (GLenum objectType, GLuint name, GLenum pname, GLint* params);
        typedef GLenum (GLAPIENTRY * PFNGLOBJECTPURGEABLEAPPLEPROC) (GLenum objectType, GLuint name, GLenum option);
        typedef GLenum (GLAPIENTRY * PFNGLOBJECTUNPURGEABLEAPPLEPROC) (GLenum objectType, GLuint name, GLenum option);

        #define glGetObjectParameterivAPPLE GLEGetCurrentFunction(glGetObjectParameterivAPPLE)
        #define glObjectPurgeableAPPLE GLEGetCurrentFunction(glObjectPurgeableAPPLE)
        #define glObjectUnpurgeableAPPLE GLEGetCurrentFunction(glObjectUnpurgeableAPPLE)

        #define GLE_APPLE_object_purgeable GLEGetCurrentVariable(gl_APPLE_object_purgeable)
    #endif



    #ifndef GL_APPLE_pixel_buffer
        #define GL_APPLE_pixel_buffer 1

        #define GL_MIN_PBUFFER_VIEWPORT_DIMS_APPLE 0x8A10

        #define GLE_APPLE_pixel_buffer GLEGetCurrentVariable(gl_APPLE_pixel_buffer)
    #endif



    #ifndef GL_APPLE_rgb_422
        #define GL_APPLE_rgb_422 1

        #define GL_UNSIGNED_SHORT_8_8_APPLE 0x85BA
        #define GL_UNSIGNED_SHORT_8_8_REV_APPLE 0x85BB
        #define GL_RGB_422_APPLE 0x8A1F
        #define GL_RGB_RAW_422_APPLE 0x8A51

        #define GLE_APPLE_rgb_422 GLEGetCurrentVariable(gl_APPLE_rgb_422)
    #endif



    #ifndef GL_APPLE_row_bytes
        #define GL_APPLE_row_bytes 1

        #define GL_PACK_ROW_BYTES_APPLE 0x8A15
        #define GL_UNPACK_ROW_BYTES_APPLE 0x8A16

        #define GLE_APPLE_row_bytes GLEGetCurrentVariable(gl_APPLE_row_bytes)
    #endif



    #ifndef GL_APPLE_specular_vector
        #define GL_APPLE_specular_vector 1

        #define GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE 0x85B0

        #define GLE_APPLE_specular_vector GLEGetCurrentVariable(gl_APPLE_specular_vector)
    #endif



    #ifndef GL_APPLE_texture_range
        #define GL_APPLE_texture_range 1

        #define GL_TEXTURE_RANGE_LENGTH_APPLE 0x85B7
        #define GL_TEXTURE_RANGE_POINTER_APPLE 0x85B8
        #define GL_TEXTURE_STORAGE_HINT_APPLE 0x85BC
        #define GL_STORAGE_PRIVATE_APPLE 0x85BD
        #define GL_STORAGE_CACHED_APPLE 0x85BE
        #define GL_STORAGE_SHARED_APPLE 0x85BF

        typedef void (GLAPIENTRY * PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC) (GLenum target, GLenum pname, void **params);
        typedef void (GLAPIENTRY * PFNGLTEXTURERANGEAPPLEPROC) (GLenum target, GLsizei length, const void *pointer);

        #define glGetTexParameterPointervAPPLE GLEGetCurrentFunction(glGetTexParameterPointervAPPLE)
        #define glTextureRangeAPPLE GLEGetCurrentFunction(glTextureRangeAPPLE)

        #define GLE_APPLE_texture_range GLEGetCurrentVariable(gl_APPLE_texture_range)
    #endif


    #ifndef GL_APPLE_transform_hint
        #define GL_APPLE_transform_hint 1

        #define GL_TRANSFORM_HINT_APPLE 0x85B1

        #define GLE_APPLE_transform_hint GLEGetCurrentVariable(gl_APPLE_transform_hint)
    #endif



    #ifndef GL_APPLE_vertex_array_object
        #define GL_APPLE_vertex_array_object 1
        
        // This has been superceded by GL_ARB_vertex_array_object, though if you are using Apple
        // OpenGL prior to 3.x then only this interface will be available. However, we have made
        // it so that glBindVertexArray maps to glBindVertexArrayApple when only the latter is present,
        // thus allowing you to write cleaner code. You can always just call glBindVertexArray instead
        // of glBindVertexArrayAPPLE, etc.
        #define GL_VERTEX_ARRAY_BINDING_APPLE 0x85B5

        typedef void      (GLAPIENTRY * PFNGLBINDVERTEXARRAYAPPLEPROC) (GLuint array);
        typedef void      (GLAPIENTRY * PFNGLDELETEVERTEXARRAYSAPPLEPROC) (GLsizei n, const GLuint* arrays);
        typedef void      (GLAPIENTRY * PFNGLGENVERTEXARRAYSAPPLEPROC) (GLsizei n, const GLuint* arrays); // It's not clear whether arrays needs to be const or not.
        typedef GLboolean (GLAPIENTRY * PFNGLISVERTEXARRAYAPPLEPROC) (GLuint array);

        #define glBindVertexArrayAPPLE    GLEGetCurrentFunction(glBindVertexArrayAPPLE)
        #define glDeleteVertexArraysAPPLE GLEGetCurrentFunction(glDeleteVertexArraysAPPLE)
        #define glGenVertexArraysAPPLE    GLEGetCurrentFunction(glGenVertexArraysAPPLE)
        #define glIsVertexArrayAPPLE      GLEGetCurrentFunction(glIsVertexArrayAPPLE)

        #define GLE_APPLE_vertex_array_object GLEGetCurrentVariable(gl_APPLE_vertex_array_object)
    #endif



    #ifndef GL_APPLE_vertex_array_range
        #define GL_APPLE_vertex_array_range 1

        #define GL_VERTEX_ARRAY_RANGE_APPLE 0x851D
        #define GL_VERTEX_ARRAY_RANGE_LENGTH_APPLE 0x851E
        #define GL_VERTEX_ARRAY_STORAGE_HINT_APPLE 0x851F
        #define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_APPLE 0x8520
        #define GL_VERTEX_ARRAY_RANGE_POINTER_APPLE 0x8521
        #define GL_STORAGE_CLIENT_APPLE 0x85B4
        #define GL_STORAGE_CACHED_APPLE 0x85BE
        #define GL_STORAGE_SHARED_APPLE 0x85BF

        typedef void (GLAPIENTRY * PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC) (GLsizei length, void *pointer);
        typedef void (GLAPIENTRY * PFNGLVERTEXARRAYPARAMETERIAPPLEPROC) (GLenum pname, GLint param);
        typedef void (GLAPIENTRY * PFNGLVERTEXARRAYRANGEAPPLEPROC) (GLsizei length, void *pointer);

        #define glFlushVertexArrayRangeAPPLE GLEGetCurrentFunction(glFlushVertexArrayRangeAPPLE)
        #define glVertexArrayParameteriAPPLE GLEGetCurrentFunction(glVertexArrayParameteriAPPLE)
        #define glVertexArrayRangeAPPLE GLEGetCurrentFunction(glVertexArrayRangeAPPLE)

        #define GLE_APPLE_vertex_array_range GLEGetCurrentVariable(gl_APPLE_vertex_array_range)
    #endif



    #ifndef GL_APPLE_vertex_program_evaluators
        #define GL_APPLE_vertex_program_evaluators 1

        #define GL_VERTEX_ATTRIB_MAP1_APPLE 0x8A00
        #define GL_VERTEX_ATTRIB_MAP2_APPLE 0x8A01
        #define GL_VERTEX_ATTRIB_MAP1_SIZE_APPLE 0x8A02
        #define GL_VERTEX_ATTRIB_MAP1_COEFF_APPLE 0x8A03
        #define GL_VERTEX_ATTRIB_MAP1_ORDER_APPLE 0x8A04
        #define GL_VERTEX_ATTRIB_MAP1_DOMAIN_APPLE 0x8A05
        #define GL_VERTEX_ATTRIB_MAP2_SIZE_APPLE 0x8A06
        #define GL_VERTEX_ATTRIB_MAP2_COEFF_APPLE 0x8A07
        #define GL_VERTEX_ATTRIB_MAP2_ORDER_APPLE 0x8A08
        #define GL_VERTEX_ATTRIB_MAP2_DOMAIN_APPLE 0x8A09

        typedef void (GLAPIENTRY * PFNGLDISABLEVERTEXATTRIBAPPLEPROC) (GLuint index, GLenum pname);
        typedef void (GLAPIENTRY * PFNGLENABLEVERTEXATTRIBAPPLEPROC) (GLuint index, GLenum pname);
        typedef GLboolean (GLAPIENTRY * PFNGLISVERTEXATTRIBENABLEDAPPLEPROC) (GLuint index, GLenum pname);
        typedef void (GLAPIENTRY * PFNGLMAPVERTEXATTRIB1DAPPLEPROC) (GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points);
        typedef void (GLAPIENTRY * PFNGLMAPVERTEXATTRIB1FAPPLEPROC) (GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points);
        typedef void (GLAPIENTRY * PFNGLMAPVERTEXATTRIB2DAPPLEPROC) (GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points);
        typedef void (GLAPIENTRY * PFNGLMAPVERTEXATTRIB2FAPPLEPROC) (GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points);

        #define glDisableVertexAttribAPPLE GLEGetCurrentFunction(glDisableVertexAttribAPPLE)
        #define glEnableVertexAttribAPPLE GLEGetCurrentFunction(glEnableVertexAttribAPPLE)
        #define glIsVertexAttribEnabledAPPLE GLEGetCurrentFunction(glIsVertexAttribEnabledAPPLE)
        #define glMapVertexAttrib1dAPPLE GLEGetCurrentFunction(glMapVertexAttrib1dAPPLE)
        #define glMapVertexAttrib1fAPPLE GLEGetCurrentFunction(glMapVertexAttrib1fAPPLE)
        #define glMapVertexAttrib2dAPPLE GLEGetCurrentFunction(glMapVertexAttrib2dAPPLE)
        #define glMapVertexAttrib2fAPPLE GLEGetCurrentFunction(glMapVertexAttrib2fAPPLE)

        #define GLE_APPLE_vertex_program_evaluators GLEGetCurrentVariable(gl_APPLE_vertex_program_evaluators)
    #endif

#endif // GLE_APPLE_ENABLED


#ifndef GL_ARB_debug_output
    #define GL_ARB_debug_output 1

    #define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB 0x8242
    #define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB 0x8243
    #define GL_DEBUG_CALLBACK_FUNCTION_ARB 0x8244
    #define GL_DEBUG_CALLBACK_USER_PARAM_ARB 0x8245
    #define GL_DEBUG_SOURCE_API_ARB 0x8246
    #define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB 0x8247
    #define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB 0x8248
    #define GL_DEBUG_SOURCE_THIRD_PARTY_ARB 0x8249
    #define GL_DEBUG_SOURCE_APPLICATION_ARB 0x824A
    #define GL_DEBUG_SOURCE_OTHER_ARB 0x824B
    #define GL_DEBUG_TYPE_ERROR_ARB 0x824C
    #define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB 0x824D
    #define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB 0x824E
    #define GL_DEBUG_TYPE_PORTABILITY_ARB 0x824F
    #define GL_DEBUG_TYPE_PERFORMANCE_ARB 0x8250
    #define GL_DEBUG_TYPE_OTHER_ARB 0x8251
    #define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB 0x9143
    #define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB 0x9144
    #define GL_DEBUG_LOGGED_MESSAGES_ARB 0x9145
    #define GL_DEBUG_SEVERITY_HIGH_ARB 0x9146
    #define GL_DEBUG_SEVERITY_MEDIUM_ARB 0x9147
    #define GL_DEBUG_SEVERITY_LOW_ARB 0x9148

    typedef void (GLAPIENTRY *GLDEBUGPROCARB)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

    typedef void (GLAPIENTRY * PFNGLDEBUGMESSAGECALLBACKARBPROC) (GLDEBUGPROCARB callback, const void *userParam);
    typedef void (GLAPIENTRY * PFNGLDEBUGMESSAGECONTROLARBPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
    typedef void (GLAPIENTRY * PFNGLDEBUGMESSAGEINSERTARBPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
    typedef GLuint (GLAPIENTRY * PFNGLGETDEBUGMESSAGELOGARBPROC) (GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);

    #define glDebugMessageCallbackARB GLEGetCurrentFunction(glDebugMessageCallbackARB)
    #define glDebugMessageControlARB GLEGetCurrentFunction(glDebugMessageControlARB)
    #define glDebugMessageInsertARB GLEGetCurrentFunction(glDebugMessageInsertARB)
    #define glGetDebugMessageLogARB GLEGetCurrentFunction(glGetDebugMessageLogARB)

    #define GLE_ARB_debug_output GLEGetCurrentVariable(gl_ARB_debug_output)

#endif // GL_ARB_debug_output




/* Disabled until needed
#ifndef GL_ARB_direct_state_access
    #define GL_ARB_direct_state_access 1

    #define GL_TEXTURE_TARGET 0x1006
    #define GL_QUERY_TARGET 0x82EA
    #define GL_TEXTURE_BINDING 0x82EB

    typedef void (GLAPIENTRY * PFNGLBINDTEXTUREUNITPROC) (GLuint unit, GLuint texture);
    typedef void (GLAPIENTRY * PFNGLBLITNAMEDFRAMEBUFFERPROC) (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    typedef GLenum (GLAPIENTRY * PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC) (GLuint framebuffer, GLenum target);
    typedef void (GLAPIENTRY * PFNGLCLEARNAMEDBUFFERDATAPROC) (GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
    typedef void (GLAPIENTRY * PFNGLCLEARNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
    typedef void (GLAPIENTRY * PFNGLCLEARNAMEDFRAMEBUFFERFIPROC) (GLuint framebuffer, GLenum buffer, GLfloat depth, GLint stencil);
    typedef void (GLAPIENTRY * PFNGLCLEARNAMEDFRAMEBUFFERFVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
    typedef void (GLAPIENTRY * PFNGLCLEARNAMEDFRAMEBUFFERIVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value);
    typedef void (GLAPIENTRY * PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC) (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
    typedef void (GLAPIENTRY * PFNGLCOPYNAMEDBUFFERSUBDATAPROC) (GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
    typedef void (GLAPIENTRY * PFNGLCOPYTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
    typedef void (GLAPIENTRY * PFNGLCOPYTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY * PFNGLCOPYTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY * PFNGLCREATEBUFFERSPROC) (GLsizei n, GLuint* buffers);
    typedef void (GLAPIENTRY * PFNGLCREATEFRAMEBUFFERSPROC) (GLsizei n, GLuint* framebuffers);
    typedef void (GLAPIENTRY * PFNGLCREATEPROGRAMPIPELINESPROC) (GLsizei n, GLuint* pipelines);
    typedef void (GLAPIENTRY * PFNGLCREATEQUERIESPROC) (GLenum target, GLsizei n, GLuint* ids);
    typedef void (GLAPIENTRY * PFNGLCREATERENDERBUFFERSPROC) (GLsizei n, GLuint* renderbuffers);
    typedef void (GLAPIENTRY * PFNGLCREATESAMPLERSPROC) (GLsizei n, GLuint* samplers);
    typedef void (GLAPIENTRY * PFNGLCREATETEXTURESPROC) (GLenum target, GLsizei n, GLuint* textures);
    typedef void (GLAPIENTRY * PFNGLCREATETRANSFORMFEEDBACKSPROC) (GLsizei n, GLuint* ids);
    typedef void (GLAPIENTRY * PFNGLCREATEVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
    typedef void (GLAPIENTRY * PFNGLDISABLEVERTEXARRAYATTRIBPROC) (GLuint vaobj, GLuint index);
    typedef void (GLAPIENTRY * PFNGLENABLEVERTEXARRAYATTRIBPROC) (GLuint vaobj, GLuint index);
    typedef void (GLAPIENTRY * PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length);
    typedef void (GLAPIENTRY * PFNGLGENERATETEXTUREMIPMAPPROC) (GLuint texture);
    typedef void (GLAPIENTRY * PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC) (GLuint texture, GLint level, GLsizei bufSize, void *pixels);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDBUFFERPARAMETERI64VPROC) (GLuint buffer, GLenum pname, GLint64* params);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDBUFFERPARAMETERIVPROC) (GLuint buffer, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDBUFFERPOINTERVPROC) (GLuint buffer, GLenum pname, void** params);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC) (GLuint framebuffer, GLenum pname, GLint* param);
    typedef void (GLAPIENTRY * PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC) (GLuint renderbuffer, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXTUREIMAGEPROC) (GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
    typedef void (GLAPIENTRY * PFNGLGETTEXTURELEVELPARAMETERFVPROC) (GLuint texture, GLint level, GLenum pname, GLfloat* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXTURELEVELPARAMETERIVPROC) (GLuint texture, GLint level, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXTUREPARAMETERIIVPROC) (GLuint texture, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXTUREPARAMETERIUIVPROC) (GLuint texture, GLenum pname, GLuint* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXTUREPARAMETERFVPROC) (GLuint texture, GLenum pname, GLfloat* params);
    typedef void (GLAPIENTRY * PFNGLGETTEXTUREPARAMETERIVPROC) (GLuint texture, GLenum pname, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETTRANSFORMFEEDBACKI64_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint64* param);
    typedef void (GLAPIENTRY * PFNGLGETTRANSFORMFEEDBACKI_VPROC) (GLuint xfb, GLenum pname, GLuint index, GLint* param);
    typedef void (GLAPIENTRY * PFNGLGETTRANSFORMFEEDBACKIVPROC) (GLuint xfb, GLenum pname, GLint* param);
    typedef void (GLAPIENTRY * PFNGLGETVERTEXARRAYINDEXED64IVPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint64* param);
    typedef void (GLAPIENTRY * PFNGLGETVERTEXARRAYINDEXEDIVPROC) (GLuint vaobj, GLuint index, GLenum pname, GLint* param);
    typedef void (GLAPIENTRY * PFNGLGETVERTEXARRAYIVPROC) (GLuint vaobj, GLenum pname, GLint* param);
    typedef void (GLAPIENTRY * PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC) (GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments);
    typedef void (GLAPIENTRY * PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC) (GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void * (GLAPIENTRY * PFNGLMAPNAMEDBUFFERPROC) (GLuint buffer, GLenum access);
    typedef void * (GLAPIENTRY * PFNGLMAPNAMEDBUFFERRANGEPROC) (GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
    typedef void (GLAPIENTRY * PFNGLNAMEDBUFFERDATAPROC) (GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
    typedef void (GLAPIENTRY * PFNGLNAMEDBUFFERSTORAGEPROC) (GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
    typedef void (GLAPIENTRY * PFNGLNAMEDBUFFERSUBDATAPROC) (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC) (GLuint framebuffer, GLenum mode);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC) (GLuint framebuffer, GLsizei n, const GLenum* bufs);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC) (GLuint framebuffer, GLenum pname, GLint param);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC) (GLuint framebuffer, GLenum mode);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC) (GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERTEXTUREPROC) (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
    typedef void (GLAPIENTRY * PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC) (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
    typedef void (GLAPIENTRY * PFNGLNAMEDRENDERBUFFERSTORAGEPROC) (GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY * PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY * PFNGLTEXTUREBUFFERPROC) (GLuint texture, GLenum internalformat, GLuint buffer);
    typedef void (GLAPIENTRY * PFNGLTEXTUREBUFFERRANGEPROC) (GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
    typedef void (GLAPIENTRY * PFNGLTEXTUREPARAMETERIIVPROC) (GLuint texture, GLenum pname, const GLint* params);
    typedef void (GLAPIENTRY * PFNGLTEXTUREPARAMETERIUIVPROC) (GLuint texture, GLenum pname, const GLuint* params);
    typedef void (GLAPIENTRY * PFNGLTEXTUREPARAMETERFPROC) (GLuint texture, GLenum pname, GLfloat param);
    typedef void (GLAPIENTRY * PFNGLTEXTUREPARAMETERFVPROC) (GLuint texture, GLenum pname, const GLfloat* param);
    typedef void (GLAPIENTRY * PFNGLTEXTUREPARAMETERIPROC) (GLuint texture, GLenum pname, GLint param);
    typedef void (GLAPIENTRY * PFNGLTEXTUREPARAMETERIVPROC) (GLuint texture, GLenum pname, const GLint* param);
    typedef void (GLAPIENTRY * PFNGLTEXTURESTORAGE1DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
    typedef void (GLAPIENTRY * PFNGLTEXTURESTORAGE2DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
    typedef void (GLAPIENTRY * PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC) (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
    typedef void (GLAPIENTRY * PFNGLTEXTURESTORAGE3DPROC) (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
    typedef void (GLAPIENTRY * PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC) (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
    typedef void (GLAPIENTRY * PFNGLTEXTURESUBIMAGE1DPROC) (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
    typedef void (GLAPIENTRY * PFNGLTEXTURESUBIMAGE2DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
    typedef void (GLAPIENTRY * PFNGLTEXTURESUBIMAGE3DPROC) (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
    typedef void (GLAPIENTRY * PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC) (GLuint xfb, GLuint index, GLuint buffer);
    typedef void (GLAPIENTRY * PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC) (GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
    typedef GLboolean (GLAPIENTRY * PFNGLUNMAPNAMEDBUFFERPROC) (GLuint buffer);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYATTRIBBINDINGPROC) (GLuint vaobj, GLuint attribindex, GLuint bindingindex);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYATTRIBFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYATTRIBIFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYATTRIBLFORMATPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYBINDINGDIVISORPROC) (GLuint vaobj, GLuint bindingindex, GLuint divisor);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYELEMENTBUFFERPROC) (GLuint vaobj, GLuint buffer);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXBUFFERPROC) (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXBUFFERSPROC) (GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr *offsets, const GLsizei *strides);

    #define glBindTextureUnit GLEGetCurrentFunction(glBindTextureUnit)
    #define glBlitNamedFramebuffer GLEGetCurrentFunction(glBlitNamedFramebuffer)
    #define glCheckNamedFramebufferStatus GLEGetCurrentFunction(glCheckNamedFramebufferStatus)
    #define glClearNamedBufferData GLEGetCurrentFunction(glClearNamedBufferData)
    #define glClearNamedBufferSubData GLEGetCurrentFunction(glClearNamedBufferSubData)
    #define glClearNamedFramebufferfi GLEGetCurrentFunction(glClearNamedFramebufferfi)
    #define glClearNamedFramebufferfv GLEGetCurrentFunction(glClearNamedFramebufferfv)
    #define glClearNamedFramebufferiv GLEGetCurrentFunction(glClearNamedFramebufferiv)
    #define glClearNamedFramebufferuiv GLEGetCurrentFunction(glClearNamedFramebufferuiv)
    #define glCompressedTextureSubImage1D GLEGetCurrentFunction(glCompressedTextureSubImage1D)
    #define glCompressedTextureSubImage2D GLEGetCurrentFunction(glCompressedTextureSubImage2D)
    #define glCompressedTextureSubImage3D GLEGetCurrentFunction(glCompressedTextureSubImage3D)
    #define glCopyNamedBufferSubData GLEGetCurrentFunction(glCopyNamedBufferSubData)
    #define glCopyTextureSubImage1D GLEGetCurrentFunction(glCopyTextureSubImage1D)
    #define glCopyTextureSubImage2D GLEGetCurrentFunction(glCopyTextureSubImage2D)
    #define glCopyTextureSubImage3D GLEGetCurrentFunction(glCopyTextureSubImage3D)
    #define glCreateBuffers GLEGetCurrentFunction(glCreateBuffers)
    #define glCreateFramebuffers GLEGetCurrentFunction(glCreateFramebuffers)
    #define glCreateProgramPipelines GLEGetCurrentFunction(glCreateProgramPipelines)
    #define glCreateQueries GLEGetCurrentFunction(glCreateQueries)
    #define glCreateRenderbuffers GLEGetCurrentFunction(glCreateRenderbuffers)
    #define glCreateSamplers GLEGetCurrentFunction(glCreateSamplers)
    #define glCreateTextures GLEGetCurrentFunction(glCreateTextures)
    #define glCreateTransformFeedbacks GLEGetCurrentFunction(glCreateTransformFeedbacks)
    #define glCreateVertexArrays GLEGetCurrentFunction(glCreateVertexArrays)
    #define glDisableVertexArrayAttrib GLEGetCurrentFunction(glDisableVertexArrayAttrib)
    #define glEnableVertexArrayAttrib GLEGetCurrentFunction(glEnableVertexArrayAttrib)
    #define glFlushMappedNamedBufferRange GLEGetCurrentFunction(glFlushMappedNamedBufferRange)
    #define glGenerateTextureMipmap GLEGetCurrentFunction(glGenerateTextureMipmap)
    #define glGetCompressedTextureImage GLEGetCurrentFunction(glGetCompressedTextureImage)
    #define glGetNamedBufferParameteri64v GLEGetCurrentFunction(glGetNamedBufferParameteri64v)
    #define glGetNamedBufferParameteriv GLEGetCurrentFunction(glGetNamedBufferParameteriv)
    #define glGetNamedBufferPointerv GLEGetCurrentFunction(glGetNamedBufferPointerv)
    #define glGetNamedBufferSubData GLEGetCurrentFunction(glGetNamedBufferSubData)
    #define glGetNamedFramebufferAttachmentParameteriv GLEGetCurrentFunction(glGetNamedFramebufferAttachmentParameteriv)
    #define glGetNamedFramebufferParameteriv GLEGetCurrentFunction(glGetNamedFramebufferParameteriv)
    #define glGetNamedRenderbufferParameteriv GLEGetCurrentFunction(glGetNamedRenderbufferParameteriv)
    #define glGetTextureImage GLEGetCurrentFunction(glGetTextureImage)
    #define glGetTextureLevelParameterfv GLEGetCurrentFunction(glGetTextureLevelParameterfv)
    #define glGetTextureLevelParameteriv GLEGetCurrentFunction(glGetTextureLevelParameteriv)
    #define glGetTextureParameterIiv GLEGetCurrentFunction(glGetTextureParameterIiv)
    #define glGetTextureParameterIuiv GLEGetCurrentFunction(glGetTextureParameterIuiv)
    #define glGetTextureParameterfv GLEGetCurrentFunction(glGetTextureParameterfv)
    #define glGetTextureParameteriv GLEGetCurrentFunction(glGetTextureParameteriv)
    #define glGetTransformFeedbacki64_v GLEGetCurrentFunction(glGetTransformFeedbacki64_v)
    #define glGetTransformFeedbacki_v GLEGetCurrentFunction(glGetTransformFeedbacki_v)
    #define glGetTransformFeedbackiv GLEGetCurrentFunction(glGetTransformFeedbackiv)
    #define glGetVertexArrayIndexed64iv GLEGetCurrentFunction(glGetVertexArrayIndexed64iv)
    #define glGetVertexArrayIndexediv GLEGetCurrentFunction(glGetVertexArrayIndexediv)
    #define glGetVertexArrayiv GLEGetCurrentFunction(glGetVertexArrayiv)
    #define glInvalidateNamedFramebufferData GLEGetCurrentFunction(glInvalidateNamedFramebufferData)
    #define glInvalidateNamedFramebufferSubData GLEGetCurrentFunction(glInvalidateNamedFramebufferSubData)
    #define glMapNamedBuffer GLEGetCurrentFunction(glMapNamedBuffer)
    #define glMapNamedBufferRange GLEGetCurrentFunction(glMapNamedBufferRange)
    #define glNamedBufferData GLEGetCurrentFunction(glNamedBufferData)
    #define glNamedBufferStorage GLEGetCurrentFunction(glNamedBufferStorage)
    #define glNamedBufferSubData GLEGetCurrentFunction(glNamedBufferSubData)
    #define glNamedFramebufferDrawBuffer GLEGetCurrentFunction(glNamedFramebufferDrawBuffer)
    #define glNamedFramebufferDrawBuffers GLEGetCurrentFunction(glNamedFramebufferDrawBuffers)
    #define glNamedFramebufferParameteri GLEGetCurrentFunction(glNamedFramebufferParameteri)
    #define glNamedFramebufferReadBuffer GLEGetCurrentFunction(glNamedFramebufferReadBuffer)
    #define glNamedFramebufferRenderbuffer GLEGetCurrentFunction(glNamedFramebufferRenderbuffer)
    #define glNamedFramebufferTexture GLEGetCurrentFunction(glNamedFramebufferTexture)
    #define glNamedFramebufferTextureLayer GLEGetCurrentFunction(glNamedFramebufferTextureLayer)
    #define glNamedRenderbufferStorage GLEGetCurrentFunction(glNamedRenderbufferStorage)
    #define glNamedRenderbufferStorageMultisample GLEGetCurrentFunction(glNamedRenderbufferStorageMultisample)
    #define glTextureBuffer GLEGetCurrentFunction(glTextureBuffer)
    #define glTextureBufferRange GLEGetCurrentFunction(glTextureBufferRange)
    #define glTextureParameterIiv GLEGetCurrentFunction(glTextureParameterIiv)
    #define glTextureParameterIuiv GLEGetCurrentFunction(glTextureParameterIuiv)
    #define glTextureParameterf GLEGetCurrentFunction(glTextureParameterf)
    #define glTextureParameterfv GLEGetCurrentFunction(glTextureParameterfv)
    #define glTextureParameteri GLEGetCurrentFunction(glTextureParameteri)
    #define glTextureParameteriv GLEGetCurrentFunction(glTextureParameteriv)
    #define glTextureStorage1D GLEGetCurrentFunction(glTextureStorage1D)
    #define glTextureStorage2D GLEGetCurrentFunction(glTextureStorage2D)
    #define glTextureStorage2DMultisample GLEGetCurrentFunction(glTextureStorage2DMultisample)
    #define glTextureStorage3D GLEGetCurrentFunction(glTextureStorage3D)
    #define glTextureStorage3DMultisample GLEGetCurrentFunction(glTextureStorage3DMultisample)
    #define glTextureSubImage1D GLEGetCurrentFunction(glTextureSubImage1D)
    #define glTextureSubImage2D GLEGetCurrentFunction(glTextureSubImage2D)
    #define glTextureSubImage3D GLEGetCurrentFunction(glTextureSubImage3D)
    #define glTransformFeedbackBufferBase GLEGetCurrentFunction(glTransformFeedbackBufferBase)
    #define glTransformFeedbackBufferRange GLEGetCurrentFunction(glTransformFeedbackBufferRange)
    #define glUnmapNamedBuffer GLEGetCurrentFunction(glUnmapNamedBuffer)
    #define glVertexArrayAttribBinding GLEGetCurrentFunction(glVertexArrayAttribBinding)
    #define glVertexArrayAttribFormat GLEGetCurrentFunction(glVertexArrayAttribFormat)
    #define glVertexArrayAttribIFormat GLEGetCurrentFunction(glVertexArrayAttribIFormat)
    #define glVertexArrayAttribLFormat GLEGetCurrentFunction(glVertexArrayAttribLFormat)
    #define glVertexArrayBindingDivisor GLEGetCurrentFunction(glVertexArrayBindingDivisor)
    #define glVertexArrayElementBuffer GLEGetCurrentFunction(glVertexArrayElementBuffer)
    #define glVertexArrayVertexBuffer GLEGetCurrentFunction(glVertexArrayVertexBuffer)
    #define glVertexArrayVertexBuffers GLEGetCurrentFunction(glVertexArrayVertexBuffers)

    #define GLE_ARB_direct_state_access GLEGetCurrentVariable(gl_ARB_direct_state_access)
#endif // GL_ARB_direct_state_access */



#ifndef GL_ARB_ES2_compatibility
    #define GL_ARB_ES2_compatibility 1

    // This is for OpenGL ES compatibility.
    #define GL_FIXED 0x140C
    #define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
    #define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
    #define GL_RGB565 0x8D62
    #define GL_LOW_FLOAT 0x8DF0
    #define GL_MEDIUM_FLOAT 0x8DF1
    #define GL_HIGH_FLOAT 0x8DF2
    #define GL_LOW_INT 0x8DF3
    #define GL_MEDIUM_INT 0x8DF4
    #define GL_HIGH_INT 0x8DF5
    #define GL_SHADER_BINARY_FORMATS 0x8DF8
    #define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
    #define GL_SHADER_COMPILER 0x8DFA
    #define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
    #define GL_MAX_VARYING_VECTORS 0x8DFC
    #define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD

    typedef int GLfixed;

    typedef void (GLAPIENTRY * PFNGLCLEARDEPTHFPROC) (GLclampf d);
    typedef void (GLAPIENTRY * PFNGLDEPTHRANGEFPROC) (GLclampf n, GLclampf f);
    typedef void (GLAPIENTRY * PFNGLGETSHADERPRECISIONFORMATPROC) (GLenum shadertype, GLenum precisiontype, GLint* range, GLint *precision);
    typedef void (GLAPIENTRY * PFNGLRELEASESHADERCOMPILERPROC) (void);
    typedef void (GLAPIENTRY * PFNGLSHADERBINARYPROC) (GLsizei count, const GLuint* shaders, GLenum binaryformat, const void*binary, GLsizei length);

    #define glClearDepthf              GLEGetCurrentFunction(glClearDepthf)
    #define glDepthRangef              GLEGetCurrentFunction(glDepthRangef)
    #define glGetShaderPrecisionFormat GLEGetCurrentFunction(glGetShaderPrecisionFormat)
    #define glReleaseShaderCompiler    GLEGetCurrentFunction(glReleaseShaderCompiler)
    #define glShaderBinary             GLEGetCurrentFunction(glShaderBinary)

    #define GLE_ARB_ES2_compatibility GLEGetCurrentVariable(gl_ARB_ES2_compatibility)
#endif



#ifndef GL_ARB_framebuffer_object
    #define GL_ARB_framebuffer_object 1

    // GL_ARB_framebuffer_object is part of the OpenGL 4.4 core profile.
    #define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
    #define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
    #define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
    #define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
    #define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
    #define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
    #define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
    #define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
    #define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
    #define GL_FRAMEBUFFER_DEFAULT 0x8218
    #define GL_FRAMEBUFFER_UNDEFINED 0x8219
    #define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
    #define GL_INDEX 0x8222
    #define GL_MAX_RENDERBUFFER_SIZE 0x84E8
    #define GL_DEPTH_STENCIL 0x84F9
    #define GL_UNSIGNED_INT_24_8 0x84FA
    #define GL_DEPTH24_STENCIL8 0x88F0
    #define GL_TEXTURE_STENCIL_SIZE 0x88F1
    #define GL_UNSIGNED_NORMALIZED 0x8C17
    #define GL_SRGB 0x8C40
    #define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
    #define GL_FRAMEBUFFER_BINDING 0x8CA6
    #define GL_RENDERBUFFER_BINDING 0x8CA7
    #define GL_READ_FRAMEBUFFER 0x8CA8
    #define GL_DRAW_FRAMEBUFFER 0x8CA9
    #define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
    #define GL_RENDERBUFFER_SAMPLES 0x8CAB
    #define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
    #define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
    #define GL_FRAMEBUFFER_COMPLETE 0x8CD5
    #define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
    #define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
    #define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
    #define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
    #define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
    #define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
    #define GL_COLOR_ATTACHMENT0 0x8CE0
    #define GL_COLOR_ATTACHMENT1 0x8CE1
    #define GL_COLOR_ATTACHMENT2 0x8CE2
    #define GL_COLOR_ATTACHMENT3 0x8CE3
    #define GL_COLOR_ATTACHMENT4 0x8CE4
    #define GL_COLOR_ATTACHMENT5 0x8CE5
    #define GL_COLOR_ATTACHMENT6 0x8CE6
    #define GL_COLOR_ATTACHMENT7 0x8CE7
    #define GL_COLOR_ATTACHMENT8 0x8CE8
    #define GL_COLOR_ATTACHMENT9 0x8CE9
    #define GL_COLOR_ATTACHMENT10 0x8CEA
    #define GL_COLOR_ATTACHMENT11 0x8CEB
    #define GL_COLOR_ATTACHMENT12 0x8CEC
    #define GL_COLOR_ATTACHMENT13 0x8CED
    #define GL_COLOR_ATTACHMENT14 0x8CEE
    #define GL_COLOR_ATTACHMENT15 0x8CEF
    #define GL_DEPTH_ATTACHMENT 0x8D00
    #define GL_STENCIL_ATTACHMENT 0x8D20
    #define GL_FRAMEBUFFER 0x8D40
    #define GL_RENDERBUFFER 0x8D41
    #define GL_RENDERBUFFER_WIDTH 0x8D42
    #define GL_RENDERBUFFER_HEIGHT 0x8D43
    #define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
    #define GL_STENCIL_INDEX1 0x8D46
    #define GL_STENCIL_INDEX4 0x8D47
    #define GL_STENCIL_INDEX8 0x8D48
    #define GL_STENCIL_INDEX16 0x8D49
    #define GL_RENDERBUFFER_RED_SIZE 0x8D50
    #define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
    #define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
    #define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
    #define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
    #define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
    #define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
    #define GL_MAX_SAMPLES 0x8D57

    typedef void      (GLAPIENTRY * PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
    typedef void      (GLAPIENTRY * PFNGLBINDRENDERBUFFERPROC) (GLenum target, GLuint renderbuffer);
    typedef void      (GLAPIENTRY * PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    typedef GLenum    (GLAPIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSPROC) (GLenum target);
    typedef void      (GLAPIENTRY * PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, const GLuint* framebuffers);
    typedef void      (GLAPIENTRY * PFNGLDELETERENDERBUFFERSPROC) (GLsizei n, const GLuint* renderbuffers);
    typedef void      (GLAPIENTRY * PFNGLFRAMEBUFFERRENDERBUFFERPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    typedef void      (GLAPIENTRY * PFNGLFRAMEBUFFERTEXTURE1DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    typedef void      (GLAPIENTRY * PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    typedef void      (GLAPIENTRY * PFNGLFRAMEBUFFERTEXTURE3DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer);
    typedef void      (GLAPIENTRY * PFNGLFRAMEBUFFERTEXTURELAYERPROC) (GLenum target,GLenum attachment, GLuint texture,GLint level,GLint layer);
    typedef void      (GLAPIENTRY * PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint* framebuffers);
    typedef void      (GLAPIENTRY * PFNGLGENRENDERBUFFERSPROC) (GLsizei n, GLuint* renderbuffers);
    typedef void      (GLAPIENTRY * PFNGLGENERATEMIPMAPPROC) (GLenum target);
    typedef void      (GLAPIENTRY * PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) (GLenum target, GLenum attachment, GLenum pname, GLint* params);
    typedef void      (GLAPIENTRY * PFNGLGETRENDERBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint* params);
    typedef GLboolean (GLAPIENTRY * PFNGLISFRAMEBUFFERPROC) (GLuint framebuffer);
    typedef GLboolean (GLAPIENTRY * PFNGLISRENDERBUFFERPROC) (GLuint renderbuffer);
    typedef void      (GLAPIENTRY * PFNGLRENDERBUFFERSTORAGEPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    typedef void      (GLAPIENTRY * PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

    #define glBindFramebuffer                     GLEGetCurrentFunction(glBindFramebuffer)
    #define glBindRenderbuffer                    GLEGetCurrentFunction(glBindRenderbuffer)
    #define glBlitFramebuffer                     GLEGetCurrentFunction(glBlitFramebuffer)
    #define glCheckFramebufferStatus              GLEGetCurrentFunction(glCheckFramebufferStatus)
    #define glDeleteFramebuffers                  GLEGetCurrentFunction(glDeleteFramebuffers)
    #define glDeleteRenderbuffers                 GLEGetCurrentFunction(glDeleteRenderbuffers)
    #define glFramebufferRenderbuffer             GLEGetCurrentFunction(glFramebufferRenderbuffer)
    #define glFramebufferTexture1D                GLEGetCurrentFunction(glFramebufferTexture1D)
    #define glFramebufferTexture2D                GLEGetCurrentFunction(glFramebufferTexture2D)
    #define glFramebufferTexture3D                GLEGetCurrentFunction(glFramebufferTexture3D)
    #define glFramebufferTextureLayer             GLEGetCurrentFunction(glFramebufferTextureLayer)
    #define glGenFramebuffers                     GLEGetCurrentFunction(glGenFramebuffers)
    #define glGenRenderbuffers                    GLEGetCurrentFunction(glGenRenderbuffers)
    #define glGenerateMipmap                      GLEGetCurrentFunction(glGenerateMipmap)
    #define glGetFramebufferAttachmentParameteriv GLEGetCurrentFunction(glGetFramebufferAttachmentParameteriv)
    #define glGetRenderbufferParameteriv          GLEGetCurrentFunction(glGetRenderbufferParameteriv)
    #define glIsFramebuffer                       GLEGetCurrentFunction(glIsFramebuffer)
    #define glIsRenderbuffer                      GLEGetCurrentFunction(glIsRenderbuffer)
    #define glRenderbufferStorage                 GLEGetCurrentFunction(glRenderbufferStorage)
    #define glRenderbufferStorageMultisample      GLEGetCurrentFunction(glRenderbufferStorageMultisample)

    #define GLE_ARB_framebuffer_object GLEGetCurrentVariable(gl_ARB_framebuffer_object)

#endif // GL_ARB_framebuffer_object



#ifndef GL_ARB_framebuffer_sRGB
    #define GL_ARB_framebuffer_sRGB 1

    // GL_ARB_framebuffer_sRGB is part of the OpenGL 4.4 core profile.
    #define GL_FRAMEBUFFER_SRGB 0x8DB9

    #define GLE_ARB_framebuffer_sRGB GLEW_GET_VAR(gle_ARB_framebuffer_sRGB)
#endif



#ifndef GL_ARB_texture_multisample
    #define GL_ARB_texture_multisample 1

    #define GL_SAMPLE_POSITION 0x8E50
    #define GL_SAMPLE_MASK 0x8E51
    #define GL_SAMPLE_MASK_VALUE 0x8E52
    #define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
    #define GL_TEXTURE_2D_MULTISAMPLE 0x9100
    #define GL_PROXY_TEXTURE_2D_MULTISAMPLE 0x9101
    #define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
    #define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
    #define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
    #define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
    #define GL_TEXTURE_SAMPLES 0x9106
    #define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
    #define GL_SAMPLER_2D_MULTISAMPLE 0x9108
    #define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
    #define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
    #define GL_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910B
    #define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
    #define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
    #define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
    #define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
    #define GL_MAX_INTEGER_SAMPLES 0x9110

    typedef void (GLAPIENTRY * PFNGLGETMULTISAMPLEFVPROC) (GLenum pname, GLuint index, GLfloat* val);
    typedef void (GLAPIENTRY * PFNGLSAMPLEMASKIPROC) (GLuint index, GLbitfield mask);
    typedef void (GLAPIENTRY * PFNGLTEXIMAGE2DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
    typedef void (GLAPIENTRY * PFNGLTEXIMAGE3DMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);

    #define glGetMultisamplefv      GLEGetCurrentFunction(glGetMultisamplefv)
    #define glSampleMaski           GLEGetCurrentFunction(glSampleMaski)
    #define glTexImage2DMultisample GLEGetCurrentFunction(glTexImage2DMultisample)
    #define glTexImage3DMultisample GLEGetCurrentFunction(glTexImage3DMultisample)

    #define GLE_ARB_texture_multisample GLEGetCurrentVariable(gl_ARB_texture_multisample)

#endif // GL_ARB_texture_multisample



#ifndef GL_ARB_texture_non_power_of_two
    #define GL_ARB_texture_non_power_of_two 1

    #define GLE_ARB_texture_non_power_of_two GLEGetCurrentVariable(gl;e_ARB_texture_non_power_of_two)
#endif



#ifndef GL_ARB_timer_query
    #define GL_ARB_timer_query 1

    #define GL_TIME_ELAPSED 0x88BF
    #define GL_TIMESTAMP 0x8E28

    typedef void (GLAPIENTRY * PFNGLGETQUERYOBJECTI64VPROC) (GLuint id, GLenum pname, GLint64* params);
    typedef void (GLAPIENTRY * PFNGLGETQUERYOBJECTUI64VPROC) (GLuint id, GLenum pname, GLuint64* params);
    typedef void (GLAPIENTRY * PFNGLQUERYCOUNTERPROC) (GLuint id, GLenum target);

    #define glGetQueryObjecti64v  GLEGetCurrentFunction(glGetQueryObjecti64v)
    #define glGetQueryObjectui64v GLEGetCurrentFunction(glGetQueryObjectui64v)
    #define glQueryCounter        GLEGetCurrentFunction(glQueryCounter)

    #define GLE_ARB_timer_query GLEGetCurrentVariable(gl_ARB_timer_query)
#endif



#ifndef GL_ARB_vertex_array_object
    #define GL_ARB_vertex_array_object 1

    #define GL_VERTEX_ARRAY_BINDING 0x85B5

    typedef void      (GLAPIENTRY * PFNGLBINDVERTEXARRAYPROC) (GLuint array);
    typedef void      (GLAPIENTRY * PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
    typedef void      (GLAPIENTRY * PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
    typedef GLboolean (GLAPIENTRY * PFNGLISVERTEXARRAYPROC) (GLuint array);

    #define glBindVertexArray    GLEGetCurrentFunction(glBindVertexArray)
    #define glDeleteVertexArrays GLEGetCurrentFunction(glDeleteVertexArrays)
    #define glGenVertexArrays    GLEGetCurrentFunction(glGenVertexArrays)
    #define glIsVertexArray      GLEGetCurrentFunction(glIsVertexArray)

    #define GLE_ARB_vertex_array_object GLEGetCurrentVariable(gl_ARB_vertex_array_object)
#endif



/* Disabled until needed
#ifndef GL_ARB_vertex_attrib_binding
    #define GL_ARB_vertex_attrib_binding 1

    #define GL_VERTEX_ATTRIB_BINDING 0x82D4
    #define GL_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D5
    #define GL_VERTEX_BINDING_DIVISOR 0x82D6
    #define GL_VERTEX_BINDING_OFFSET 0x82D7
    #define GL_VERTEX_BINDING_STRIDE 0x82D8
    #define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
    #define GL_MAX_VERTEX_ATTRIB_BINDINGS 0x82DA
    #define GL_VERTEX_BINDING_BUFFER 0x8F4F

    typedef void (GLAPIENTRY * PFNGLBINDVERTEXBUFFERPROC) (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC) (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC) (GLuint vaobj, GLuint attribindex, GLuint bindingindex);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC) (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC) (GLuint vaobj, GLuint bindingindex, GLuint divisor);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBBINDINGPROC) (GLuint attribindex, GLuint bindingindex);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBIFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXATTRIBLFORMATPROC) (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
    typedef void (GLAPIENTRY * PFNGLVERTEXBINDINGDIVISORPROC) (GLuint bindingindex, GLuint divisor);

    #define glBindVertexBuffer GLEGetCurrentFunction(glBindVertexBuffer)
    #define glVertexArrayBindVertexBufferEXT GLEGetCurrentFunction(glVertexArrayBindVertexBufferEXT)
    #define glVertexArrayVertexAttribBindingEXT GLEGetCurrentFunction(glVertexArrayVertexAttribBindingEXT)
    #define glVertexArrayVertexAttribFormatEXT GLEGetCurrentFunction(glVertexArrayVertexAttribFormatEXT)
    #define glVertexArrayVertexAttribIFormatEXT GLEGetCurrentFunction(glVertexArrayVertexAttribIFormatEXT)
    #define glVertexArrayVertexAttribLFormatEXT GLEGetCurrentFunction(glVertexArrayVertexAttribLFormatEXT)
    #define glVertexArrayVertexBindingDivisorEXT GLEGetCurrentFunction(glVertexArrayVertexBindingDivisorEXT)
    #define glVertexAttribBinding GLEGetCurrentFunction(glVertexAttribBinding)
    #define glVertexAttribFormat GLEGetCurrentFunction(glVertexAttribFormat)
    #define glVertexAttribIFormat GLEGetCurrentFunction(glVertexAttribIFormat)
    #define glVertexAttribLFormat GLEGetCurrentFunction(glVertexAttribLFormat)
    #define glVertexBindingDivisor GLEGetCurrentFunction(glVertexBindingDivisor)

    #define GLE_ARB_vertex_attrib_binding GLEGetCurrentVariable(gl_ARB_vertex_attrib_binding)
#endif
*/


#ifndef GL_EXT_draw_buffers2
    #define GL_EXT_draw_buffers2 1

    typedef void      (GLAPIENTRY * PFNGLCOLORMASKINDEXEDEXTPROC) (GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
    typedef void      (GLAPIENTRY * PFNGLDISABLEINDEXEDEXTPROC) (GLenum target, GLuint index);
    typedef void      (GLAPIENTRY * PFNGLENABLEINDEXEDEXTPROC) (GLenum target, GLuint index);
    typedef void      (GLAPIENTRY * PFNGLGETBOOLEANINDEXEDVEXTPROC) (GLenum value, GLuint index, GLboolean* data);
    typedef void      (GLAPIENTRY * PFNGLGETINTEGERINDEXEDVEXTPROC) (GLenum value, GLuint index, GLint* data);
    typedef GLboolean (GLAPIENTRY * PFNGLISENABLEDINDEXEDEXTPROC) (GLenum target, GLuint index);

    #define glColorMaskIndexedEXT   GLEGetCurrentFunction(glColorMaskIndexedEXT)
    #define glDisableIndexedEXT     GLEGetCurrentFunction(glDisableIndexedEXT)
    #define glEnableIndexedEXT      GLEGetCurrentFunction(glEnableIndexedEXT)
    #define glGetBooleanIndexedvEXT GLEGetCurrentFunction(glGetBooleanIndexedvEXT)
    #define glGetIntegerIndexedvEXT GLEGetCurrentFunction(glGetIntegerIndexedvEXT)
    #define glIsEnabledIndexedEXT   GLEGetCurrentFunction(glIsEnabledIndexedEXT)

    #define GLE_EXT_draw_buffers2 GLEGetCurrentVariable(gl_EXT_draw_buffers2)
#endif



#ifndef GL_EXT_texture_filter_anisotropic
    #define GL_EXT_texture_filter_anisotropic 1

    #define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
    #define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

    #define GLE_EXT_texture_filter_anisotropic GLEW_GET_VAR(gle_EXT_texture_filter_anisotropic)
#endif



/* Disabled until needed
#ifndef GL_KHR_context_flush_control
    #define GL_KHR_context_flush_control 1

    #define GLE_KHR_context_flush_control GLEGetCurrentVariable(gl_KHR_context_flush_control)
#endif
*/



#ifndef GL_KHR_debug
    #define GL_KHR_debug 1

    #define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
    #define GL_STACK_OVERFLOW 0x0503
    #define GL_STACK_UNDERFLOW 0x0504
    #define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
    #define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
    #define GL_DEBUG_CALLBACK_FUNCTION 0x8244
    #define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
    #define GL_DEBUG_SOURCE_API 0x8246
    #define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
    #define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
    #define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
    #define GL_DEBUG_SOURCE_APPLICATION 0x824A
    #define GL_DEBUG_SOURCE_OTHER 0x824B
    #define GL_DEBUG_TYPE_ERROR 0x824C
    #define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
    #define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
    #define GL_DEBUG_TYPE_PORTABILITY 0x824F
    #define GL_DEBUG_TYPE_PERFORMANCE 0x8250
    #define GL_DEBUG_TYPE_OTHER 0x8251
    #define GL_DEBUG_TYPE_MARKER 0x8268
    #define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
    #define GL_DEBUG_TYPE_POP_GROUP 0x826A
    #define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
    #define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
    #define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
    #define GL_BUFFER 0x82E0
    #define GL_SHADER 0x82E1
    #define GL_PROGRAM 0x82E2
    #define GL_QUERY 0x82E3
    #define GL_PROGRAM_PIPELINE 0x82E4
    #define GL_SAMPLER 0x82E6
    #define GL_DISPLAY_LIST 0x82E7
    #define GL_MAX_LABEL_LENGTH 0x82E8
    #define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
    #define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
    #define GL_DEBUG_LOGGED_MESSAGES 0x9145
    #define GL_DEBUG_SEVERITY_HIGH 0x9146
    #define GL_DEBUG_SEVERITY_MEDIUM 0x9147
    #define GL_DEBUG_SEVERITY_LOW 0x9148
    #define GL_DEBUG_OUTPUT 0x92E0

    typedef void (GLAPIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

    typedef void   (GLAPIENTRY * PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, const void *userParam);
    typedef void   (GLAPIENTRY * PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
    typedef void   (GLAPIENTRY * PFNGLDEBUGMESSAGEINSERTPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
    typedef GLuint (GLAPIENTRY * PFNGLGETDEBUGMESSAGELOGPROC) (GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);
    typedef void   (GLAPIENTRY * PFNGLGETOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar *label);
    typedef void   (GLAPIENTRY * PFNGLGETOBJECTPTRLABELPROC) (const void *ptr, GLsizei bufSize, GLsizei* length, GLchar *label);
    typedef void   (GLAPIENTRY * PFNGLOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei length, const GLchar* label);
    typedef void   (GLAPIENTRY * PFNGLOBJECTPTRLABELPROC) (const void *ptr, GLsizei length, const GLchar* label);
    typedef void   (GLAPIENTRY * PFNGLPOPDEBUGGROUPPROC) (void);
    typedef void   (GLAPIENTRY * PFNGLPUSHDEBUGGROUPPROC) (GLenum source, GLuint id, GLsizei length, const GLchar * message);

    #define glDebugMessageCallback GLEGetCurrentFunction(glDebugMessageCallback)
    #define glDebugMessageControl GLEGetCurrentFunction(glDebugMessageControl)
    #define glDebugMessageInsert GLEGetCurrentFunction(glDebugMessageInsert)
    #define glGetDebugMessageLog GLEGetCurrentFunction(glGetDebugMessageLog)
    #define glGetObjectLabel GLEGetCurrentFunction(glGetObjectLabel)
    #define glGetObjectPtrLabel GLEGetCurrentFunction(glGetObjectPtrLabel)
    #define glObjectLabel GLEGetCurrentFunction(glObjectLabel)
    #define glObjectPtrLabel GLEGetCurrentFunction(glObjectPtrLabel)
    #define glPopDebugGroup GLEGetCurrentFunction(glPopDebugGroup)
    #define glPushDebugGroup GLEGetCurrentFunction(glPushDebugGroup)

    #define GLE_KHR_debug GLEGetCurrentVariable(gl_KHR_debug)
#endif // GL_KHR_debug



#ifndef GL_KHR_robust_buffer_access_behavior
    #define GL_KHR_robust_buffer_access_behavior 1

    #define GLE_KHR_robust_buffer_access_behavior GLEGetCurrentVariable(gl_KHR_robust_buffer_access_behavior)
#endif



/* Disabled until needed
#ifndef GL_KHR_robustness
    #define GL_KHR_robustness 1

    #define GL_CONTEXT_LOST 0x0507
    #define GL_LOSE_CONTEXT_ON_RESET 0x8252
    #define GL_GUILTY_CONTEXT_RESET 0x8253
    #define GL_INNOCENT_CONTEXT_RESET 0x8254
    #define GL_UNKNOWN_CONTEXT_RESET 0x8255
    #define GL_RESET_NOTIFICATION_STRATEGY 0x8256
    #define GL_NO_RESET_NOTIFICATION 0x8261
    #define GL_CONTEXT_ROBUST_ACCESS 0x90F3

    typedef void (GLAPIENTRY * PFNGLGETNUNIFORMFVPROC) (GLuint program, GLint location, GLsizei bufSize, GLfloat* params);
    typedef void (GLAPIENTRY * PFNGLGETNUNIFORMIVPROC) (GLuint program, GLint location, GLsizei bufSize, GLint* params);
    typedef void (GLAPIENTRY * PFNGLGETNUNIFORMUIVPROC) (GLuint program, GLint location, GLsizei bufSize, GLuint* params);
    typedef void (GLAPIENTRY * PFNGLREADNPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);

    #define glGetnUniformfv  GLEGetCurrentFunction(glGetnUniformfv)
    #define glGetnUniformiv  GLEGetCurrentFunction(glGetnUniformiv)
    #define glGetnUniformuiv GLEGetCurrentFunction(glGetnUniformuiv)
    #define glReadnPixels    GLEGetCurrentFunction(glReadnPixels)

    #define GLE_KHR_robustness GLEGetCurrentVariable(gl_KHR_robustness)

#endif // GL_KHR_robustness
*/



#ifndef GL_WIN_swap_hint
    #define GL_WIN_swap_hint 1

    typedef void (GLAPIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

    #define glAddSwapHintRectWIN GLEGetCurrentFunction(glAddSwapHintRectWIN)

    #define GLE_WIN_swap_hint GLEGetCurrentVariable(gl_WIN_swap_hint)
#endif



/************************************************************************************
   Windows-specific (WGL) functionality
************************************************************************************/

#if defined(GLE_WINDOWS_ENABLED)
    #ifdef __wglext_h_
        #error wglext.h was included before this header. This header needs to be inlcuded instead of or at least before wglext.h
    #endif
    #define __wglext_h_ // Prevent wglext.h from having any future effect if it's #included.

    // Declare shared types and structs from wglext.h
    DECLARE_HANDLE(HPBUFFERARB); // This type is used by a couple extensions.

    // WGL functions from <wingdi.h>
    #if defined(GLE_HOOKING_ENABLED)
        #define wglCopyContext(...)              GLEGetCurrentFunction(wglCopyContext)(__VA_ARGS__)
        #define wglCreateContext(...)            GLEGetCurrentFunction(wglCreateContext)(__VA_ARGS__)
        #define wglCreateLayerContext(...)       GLEGetCurrentFunction(wglCreateLayerContext)(__VA_ARGS__)
        #define wglDeleteContext(...)            GLEGetCurrentFunction(wglDeleteContext)(__VA_ARGS__)
        #define wglGetCurrentContext(...)        GLEGetCurrentFunction(wglGetCurrentContext)(__VA_ARGS__)
        #define wglGetCurrentDC(...)             GLEGetCurrentFunction(wglGetCurrentDC)(__VA_ARGS__)
        #define wglGetProcAddress(...)           GLEGetCurrentFunction(wglGetProcAddress)(__VA_ARGS__)
        #define wglMakeCurrent(...)              GLEGetCurrentFunction(wglMakeCurrent)(__VA_ARGS__)
        #define wglShareLists(...)               GLEGetCurrentFunction(wglShareLists)(__VA_ARGS__)
        #define wglUseFontBitmapsA(...)          GLEGetCurrentFunction(wglUseFontBitmapsA)(__VA_ARGS__)
        #define wglUseFontBitmapsW(...)          GLEGetCurrentFunction(wglUseFontBitmapsW)(__VA_ARGS__)
        #define wglUseFontOutlinesA(...)         GLEGetCurrentFunction(wglUseFontOutlinesA)(__VA_ARGS__)
        #define wglUseFontOutlinesW(...)         GLEGetCurrentFunction(wglUseFontOutlinesW)(__VA_ARGS__)
        #define wglDescribeLayerPlane(...)       GLEGetCurrentFunction(wglDescribeLayerPlane)(__VA_ARGS__)
        #define wglSetLayerPaletteEntries(...)   GLEGetCurrentFunction(wglSetLayerPaletteEntries)(__VA_ARGS__)
        #define wglGetLayerPaletteEntries(...)   GLEGetCurrentFunction(wglGetLayerPaletteEntries)(__VA_ARGS__)
        #define wglRealizeLayerPalette(...)      GLEGetCurrentFunction(wglRealizeLayerPalette)(__VA_ARGS__)
        #define wglSwapLayerBuffers(...)         GLEGetCurrentFunction(wglSwapLayerBuffers)(__VA_ARGS__)
        #define wglSwapMultipleBuffers(...)      GLEGetCurrentFunction(wglSwapMultipleBuffers)(__VA_ARGS__)
    #else
        // The following functions are directly declared in Microsoft's <wingdi.h> without associated typedefs, and are exported from Opengl32.dll.
        // We can link to them directly through Opengl32.lib/dll (same as OpenGL 1.1 functions) or we can dynamically link them from OpenGL32.dll at runtime.
        /*
        WINGDIAPI BOOL  WINAPI wglCopyContext(HGLRC, HGLRC, UINT);
        WINGDIAPI HGLRC WINAPI wglCreateContext(HDC);
        WINGDIAPI HGLRC WINAPI wglCreateLayerContext(HDC, int);
        WINGDIAPI BOOL  WINAPI wglDeleteContext(HGLRC);
        WINGDIAPI HGLRC WINAPI wglGetCurrentContext(VOID);
        WINGDIAPI HDC   WINAPI wglGetCurrentDC(VOID);
        WINGDIAPI PROC  WINAPI wglGetProcAddress(LPCSTR);
        WINGDIAPI BOOL  WINAPI wglMakeCurrent(HDC, HGLRC);
        WINGDIAPI BOOL  WINAPI wglShareLists(HGLRC, HGLRC);
        WINGDIAPI BOOL  WINAPI wglUseFontBitmapsA(HDC, DWORD, DWORD, DWORD);
        WINGDIAPI BOOL  WINAPI wglUseFontBitmapsW(HDC, DWORD, DWORD, DWORD);
        WINGDIAPI BOOL  WINAPI wglUseFontOutlinesA(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
        WINGDIAPI BOOL  WINAPI wglUseFontOutlinesW(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
        WINGDIAPI BOOL  WINAPI wglDescribeLayerPlane(HDC, int, int, UINT, LPLAYERPLANEDESCRIPTOR);
        WINGDIAPI int   WINAPI wglSetLayerPaletteEntries(HDC, int, int, int, CONST COLORREF *);
        WINGDIAPI int   WINAPI wglGetLayerPaletteEntries(HDC, int, int, int, COLORREF *);
        WINGDIAPI BOOL  WINAPI wglRealizeLayerPalette(HDC, int, BOOL);
        WINGDIAPI BOOL  WINAPI wglSwapLayerBuffers(HDC, UINT);
        WINGDIAPI DWORD WINAPI wglSwapMultipleBuffers(UINT, CONST WGLSWAP *);
        */
    #endif

    // Note: In order to detect the WGL extensions' availability, we need to call wglGetExtensionsStringARB or 
    // wglGetExtensionsStringEXT instead of glGetString(GL_EXTENSIONS).
    #ifndef WGL_ARB_buffer_region
        #define WGL_ARB_buffer_region 1

        #define WGL_FRONT_COLOR_BUFFER_BIT_ARB 0x00000001
        #define WGL_BACK_COLOR_BUFFER_BIT_ARB  0x00000002
        #define WGL_DEPTH_BUFFER_BIT_ARB       0x00000004
        #define WGL_STENCIL_BUFFER_BIT_ARB     0x00000008

        typedef HANDLE (WINAPI * PFNWGLCREATEBUFFERREGIONARBPROC) (HDC hDC, int iLayerPlane, UINT uType);
        typedef VOID   (WINAPI * PFNWGLDELETEBUFFERREGIONARBPROC) (HANDLE hRegion);
        typedef BOOL   (WINAPI * PFNWGLSAVEBUFFERREGIONARBPROC) (HANDLE hRegion, int x, int y, int width, int height);
        typedef BOOL   (WINAPI * PFNWGLRESTOREBUFFERREGIONARBPROC) (HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);

        #define wglCreateBufferRegionARB  GLEGetCurrentFunction(wglCreateBufferRegionARB)
        #define wglDeleteBufferRegionARB  GLEGetCurrentFunction(wglDeleteBufferRegionARB)
        #define wglSaveBufferRegionARB    GLEGetCurrentFunction(wglSaveBufferRegionARB)
        #define wglRestoreBufferRegionARB GLEGetCurrentFunction(wglRestoreBufferRegionARB)

        #define GLE_WGL_ARB_buffer_region GLEGetCurrentVariable(gl_WGL_ARB_buffer_region)
    #endif


    #ifndef WGL_ARB_extensions_string
        #define WGL_ARB_extensions_string 1

        typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);

        #define wglGetExtensionsStringARB  GLEGetCurrentFunction(wglGetExtensionsStringARB)

        #define GLE_WGL_ARB_extensions_string GLEGetCurrentVariable(gl_WGL_ARB_extensions_string)
    #endif


    #ifndef WGL_ARB_pixel_format
        #define WGL_ARB_pixel_format 1

        #define WGL_NUMBER_PIXEL_FORMATS_ARB   0x2000
        #define WGL_DRAW_TO_WINDOW_ARB         0x2001
        #define WGL_DRAW_TO_BITMAP_ARB         0x2002
        #define WGL_ACCELERATION_ARB           0x2003
        #define WGL_NEED_PALETTE_ARB           0x2004
        #define WGL_NEED_SYSTEM_PALETTE_ARB    0x2005
        #define WGL_SWAP_LAYER_BUFFERS_ARB     0x2006
        #define WGL_SWAP_METHOD_ARB            0x2007
        #define WGL_NUMBER_OVERLAYS_ARB        0x2008
        #define WGL_NUMBER_UNDERLAYS_ARB       0x2009
        #define WGL_TRANSPARENT_ARB            0x200A
        #define WGL_TRANSPARENT_RED_VALUE_ARB  0x2037
        #define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
        #define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
        #define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
        #define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
        #define WGL_SHARE_DEPTH_ARB            0x200C
        #define WGL_SHARE_STENCIL_ARB          0x200D
        #define WGL_SHARE_ACCUM_ARB            0x200E
        #define WGL_SUPPORT_GDI_ARB            0x200F
        #define WGL_SUPPORT_OPENGL_ARB         0x2010
        #define WGL_DOUBLE_BUFFER_ARB          0x2011
        #define WGL_STEREO_ARB                 0x2012
        #define WGL_PIXEL_TYPE_ARB             0x2013
        #define WGL_COLOR_BITS_ARB             0x2014
        #define WGL_RED_BITS_ARB               0x2015
        #define WGL_RED_SHIFT_ARB              0x2016
        #define WGL_GREEN_BITS_ARB             0x2017
        #define WGL_GREEN_SHIFT_ARB            0x2018
        #define WGL_BLUE_BITS_ARB              0x2019
        #define WGL_BLUE_SHIFT_ARB             0x201A
        #define WGL_ALPHA_BITS_ARB             0x201B
        #define WGL_ALPHA_SHIFT_ARB            0x201C
        #define WGL_ACCUM_BITS_ARB             0x201D
        #define WGL_ACCUM_RED_BITS_ARB         0x201E
        #define WGL_ACCUM_GREEN_BITS_ARB       0x201F
        #define WGL_ACCUM_BLUE_BITS_ARB        0x2020
        #define WGL_ACCUM_ALPHA_BITS_ARB       0x2021
        #define WGL_DEPTH_BITS_ARB             0x2022
        #define WGL_STENCIL_BITS_ARB           0x2023
        #define WGL_AUX_BUFFERS_ARB            0x2024
        #define WGL_NO_ACCELERATION_ARB        0x2025
        #define WGL_GENERIC_ACCELERATION_ARB   0x2026
        #define WGL_FULL_ACCELERATION_ARB      0x2027
        #define WGL_SWAP_EXCHANGE_ARB          0x2028
        #define WGL_SWAP_COPY_ARB              0x2029
        #define WGL_SWAP_UNDEFINED_ARB         0x202A
        #define WGL_TYPE_RGBA_ARB              0x202B
        #define WGL_TYPE_COLORINDEX_ARB        0x202C

        typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
        typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
        typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

        #define wglGetPixelFormatAttribivARB  GLEGetCurrentFunction(wglGetPixelFormatAttribivARB)
        #define wglGetPixelFormatAttribfvARB  GLEGetCurrentFunction(wglGetPixelFormatAttribfvARB)
        #define wglChoosePixelFormatARB  GLEGetCurrentFunction(wglChoosePixelFormatARB)

        #define GLE_WGL_ARB_pixel_format GLEGetCurrentVariable(gl_WGL_ARB_pixel_format)
    #endif


    #ifndef WGL_ARB_make_current_read
        #define WGL_ARB_make_current_read 1

        #define ERROR_INVALID_PIXEL_TYPE_ARB   0x2043
        #define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054

        typedef BOOL (WINAPI * PFNWGLMAKECONTEXTCURRENTARBPROC) (HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
        typedef HDC  (WINAPI * PFNWGLGETCURRENTREADDCARBPROC) (void);

        #define wglMakeContextCurrentARB  GLEGetCurrentFunction(wglMakeContextCurrentARB)
        #define wglGetCurrentReadDCARB  GLEGetCurrentFunction(wglGetCurrentReadDCARB)

        #define GLE_WGL_ARB_make_current_read GLEGetCurrentVariable(gl_WGL_ARB_make_current_read)
    #endif


    #ifndef WGL_ARB_pbuffer
        #define WGL_ARB_pbuffer 1

        #define WGL_DRAW_TO_PBUFFER_ARB        0x202D
        #define WGL_MAX_PBUFFER_PIXELS_ARB     0x202E
        #define WGL_MAX_PBUFFER_WIDTH_ARB      0x202F
        #define WGL_MAX_PBUFFER_HEIGHT_ARB     0x2030
        #define WGL_PBUFFER_LARGEST_ARB        0x2033
        #define WGL_PBUFFER_WIDTH_ARB          0x2034
        #define WGL_PBUFFER_HEIGHT_ARB         0x2035

        typedef HPBUFFERARB (WINAPI * PFNWGLCREATEPBUFFERARBPROC) (HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList);
        typedef HDC         (WINAPI * PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
        typedef int         (WINAPI * PFNWGLRELEASEPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer, HDC hDC);
        typedef BOOL        (WINAPI * PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
        typedef BOOL        (WINAPI * PFNWGLQUERYPBUFFERARBPROC) (HPBUFFERARB hPbuffer, int iAttribute, int *piValue);

        #define wglCreatePbufferARB  GLEGetCurrentFunction(wglCreatePbufferARB)
        #define wglGetPbufferDCARB  GLEGetCurrentFunction(wglGetPbufferDCARB)
        #define wglReleasePbufferDCARB  GLEGetCurrentFunction(wglReleasePbufferDCARB)
        #define wglDestroyPbufferARB  GLEGetCurrentFunction(wglDestroyPbufferARB)
        #define wglQueryPbufferARB  GLEGetCurrentFunction(wglQueryPbufferARB)

        #define GLE_WGL_ARB_pbuffer GLEGetCurrentVariable(gl_WGL_ARB_pbuffer)
    #endif


    #ifndef WGL_ARB_render_texture
        #define WGL_ARB_render_texture 1

        #define WGL_BIND_TO_TEXTURE_RGB_ARB         0x2070
        #define WGL_BIND_TO_TEXTURE_RGBA_ARB        0x2071
        #define WGL_TEXTURE_FORMAT_ARB              0x2072
        #define WGL_TEXTURE_TARGET_ARB              0x2073
        #define WGL_MIPMAP_TEXTURE_ARB              0x2074
        #define WGL_TEXTURE_RGB_ARB                 0x2075
        #define WGL_TEXTURE_RGBA_ARB                0x2076
        #define WGL_NO_TEXTURE_ARB                  0x2077
        #define WGL_TEXTURE_CUBE_MAP_ARB            0x2078
        #define WGL_TEXTURE_1D_ARB                  0x2079
        #define WGL_TEXTURE_2D_ARB                  0x207A
        #define WGL_MIPMAP_LEVEL_ARB                0x207B
        #define WGL_CUBE_MAP_FACE_ARB               0x207C
        #define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x207D
        #define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x207E
        #define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x207F
        #define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x2080
        #define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x2081
        #define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x2082
        #define WGL_FRONT_LEFT_ARB                  0x2083
        #define WGL_FRONT_RIGHT_ARB                 0x2084
        #define WGL_BACK_LEFT_ARB                   0x2085
        #define WGL_BACK_RIGHT_ARB                  0x2086
        #define WGL_AUX0_ARB                        0x2087
        #define WGL_AUX1_ARB                        0x2088
        #define WGL_AUX2_ARB                        0x2089
        #define WGL_AUX3_ARB                        0x208A
        #define WGL_AUX4_ARB                        0x208B
        #define WGL_AUX5_ARB                        0x208C
        #define WGL_AUX6_ARB                        0x208D
        #define WGL_AUX7_ARB                        0x208E
        #define WGL_AUX8_ARB                        0x208F
        #define WGL_AUX9_ARB                        0x2090

        typedef BOOL (WINAPI * PFNWGLBINDTEXIMAGEARBPROC) (HPBUFFERARB hPbuffer, int iBuffer);
        typedef BOOL (WINAPI * PFNWGLRELEASETEXIMAGEARBPROC) (HPBUFFERARB hPbuffer, int iBuffer);
        typedef BOOL (WINAPI * PFNWGLSETPBUFFERATTRIBARBPROC) (HPBUFFERARB hPbuffer, const int *piAttribList);

        #define wglBindTexImageARB     GLEGetCurrentFunction(wglBindTexImageARB)
        #define wglReleaseTexImageARB  GLEGetCurrentFunction(wglReleaseTexImageARB)
        #define wglSetPbufferAttribARB GLEGetCurrentFunction(wglSetPbufferAttribARB)

        #define GLE_WGL_ARB_render_texture GLEGetCurrentVariable(gl_WGL_ARB_render_texture)
    #endif


    #ifndef WGL_ARB_pixel_format_float
        #define WGL_ARB_pixel_format_float 1

        #define WGL_TYPE_RGBA_FLOAT_ARB        0x21A0

        #define GLE_WGL_TYPE_RGBA_FLOAT_ARB GLEGetCurrentVariable(gl_WGL_TYPE_RGBA_FLOAT_ARB)
    #endif


    #ifndef WGL_ARB_framebuffer_sRGB
        #define WGL_ARB_framebuffer_sRGB 1

        #define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20A9

        #define GLE_WGL_ARB_framebuffer_sRGB GLEGetCurrentVariable(gl_WGL_ARB_framebuffer_sRGB)
    #endif


    #ifndef WGL_NV_present_video
        #define WGL_NV_present_video 1

        DECLARE_HANDLE(HVIDEOOUTPUTDEVICENV);

        typedef int  (WINAPI * PFNWGLENUMERATEVIDEODEVICESNVPROC) (HDC hDC, HVIDEOOUTPUTDEVICENV *phDeviceList);
        typedef BOOL (WINAPI * PFNWGLBINDVIDEODEVICENVPROC) (HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int *piAttribList);
        typedef BOOL (WINAPI * PFNWGLQUERYCURRENTCONTEXTNVPROC) (int iAttribute, int *piValue);

        #define wglEnumerateVideoDevicesNV GLEGetCurrentFunction(wglEnumerateVideoDevicesNV)
        #define wglBindVideoDeviceNV       GLEGetCurrentFunction(wglBindVideoDeviceNV)
        #define wglQueryCurrentContextNV   GLEGetCurrentFunction(wglQueryCurrentContextNV)

        #define GLE_WGL_NV_present_video GLEGetCurrentVariable(gl_WGL_NV_present_video)
    #endif


    #ifndef WGL_ARB_create_context
        #define WGL_ARB_create_context 1

        #define WGL_CONTEXT_DEBUG_BIT_ARB      0x00000001
        #define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
        #define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
        #define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
        #define WGL_CONTEXT_LAYER_PLANE_ARB    0x2093
        #define WGL_CONTEXT_FLAGS_ARB          0x2094
        #define ERROR_INVALID_VERSION_ARB      0x2095

        typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

        #define wglCreateContextAttribsARB  GLEGetCurrentFunction(wglCreateContextAttribsARB)

        #define GLE_WGL_ARB_create_context GLEGetCurrentVariable(gl_WGL_ARB_create_context)
    #endif


    #ifndef WGL_ARB_create_context_profile
        #define WGL_ARB_create_context_profile 1

        #define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126
        #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
        #define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
        #define ERROR_INVALID_PROFILE_ARB      0x2096

        #define GLE_WGL_ARB_create_context_profile GLEGetCurrentVariable(gl_WGL_ARB_create_context_profile)
    #endif


    #ifndef WGL_ARB_create_context_robustness
        #define WGL_ARB_create_context_robustness 1

        #define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
        #define WGL_LOSE_CONTEXT_ON_RESET_ARB  0x8252
        #define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
        #define WGL_NO_RESET_NOTIFICATION_ARB  0x8261

        #define GLE_WGL_ARB_create_context_robustness GLEGetCurrentVariable(gl_WGL_ARB_create_context_robustness)
    #endif


    #ifndef WGL_EXT_extensions_string
        #define WGL_EXT_extensions_string 1

        typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);

        #define wglGetExtensionsStringEXT  GLEGetCurrentFunction(wglGetExtensionsStringEXT)

        #define GLE_WGL_EXT_extensions_string GLEGetCurrentVariable(gl_WGL_EXT_extensions_string)
    #endif


    #ifndef WGL_EXT_swap_control
        #define WGL_EXT_swap_control 1

        typedef int  (WINAPI * PFNWGLGETSWAPINTERVALEXTPROC) (void);
        typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);

        #define wglGetSwapIntervalEXT GLEGetCurrentFunction(wglGetSwapIntervalEXT)
        #define wglSwapIntervalEXT    GLEGetCurrentFunction(wglSwapIntervalEXT)

        #define GLE_WGL_EXT_swap_control GLEGetCurrentVariable(gl_WGL_EXT_swap_control)
    #endif


    #ifndef WGL_OML_sync_control
        #define WGL_OML_sync_control 1

        typedef BOOL  (WINAPI * PFNWGLGETSYNCVALUESOMLPROC) (HDC hdc, INT64 *ust, INT64 *msc, INT64 *sbc);
        typedef BOOL  (WINAPI * PFNWGLGETMSCRATEOMLPROC) (HDC hdc, INT32 *numerator, INT32 *denominator);
        typedef INT64 (WINAPI * PFNWGLSWAPBUFFERSMSCOMLPROC) (HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder);
        typedef INT64 (WINAPI * PFNWGLSWAPLAYERBUFFERSMSCOMLPROC) (HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder);
        typedef BOOL  (WINAPI * PFNWGLWAITFORMSCOMLPROC) (HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 *ust, INT64 *msc, INT64 *sbc);
        typedef BOOL  (WINAPI * PFNWGLWAITFORSBCOMLPROC) (HDC hdc, INT64 target_sbc, INT64 *ust, INT64 *msc, INT64 *sbc);

        #define wglGetSyncValuesOML       GLEGetCurrentFunction(wglGetSyncValuesOML)
        #define wglGetMscRateOML          GLEGetCurrentFunction(wglGetMscRateOML)
        #define wglSwapBuffersMscOML      GLEGetCurrentFunction(wglSwapBuffersMscOML)
        #define wglSwapLayerBuffersMscOML GLEGetCurrentFunction(wglSwapLayerBuffersMscOML)
        #define wglWaitForMscOML          GLEGetCurrentFunction(wglWaitForMscOML)
        #define wglWaitForSbcOML          GLEGetCurrentFunction(wglWaitForSbcOML)

        #define GLE_WGL_OML_sync_control GLEGetCurrentVariable(gl_WGL_OML_sync_control)
    #endif


    #ifndef WGL_EXT_framebuffer_sRGB
        #define WGL_EXT_framebuffer_sRGB 1

        #define WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT 0x20A9

        #define GLE_WGL_EXT_framebuffer_sRGB GLEGetCurrentVariable(gl_WGL_EXT_framebuffer_sRGB)
    #endif


    #ifndef WGL_NV_video_output
        #define WGL_NV_video_output 1

        DECLARE_HANDLE(HPVIDEODEV);

        typedef BOOL (WINAPI * PFNWGLGETVIDEODEVICENVPROC) (HDC hDC, int numDevices, HPVIDEODEV *hVideoDevice);
        typedef BOOL (WINAPI * PFNWGLRELEASEVIDEODEVICENVPROC) (HPVIDEODEV hVideoDevice);
        typedef BOOL (WINAPI * PFNWGLBINDVIDEOIMAGENVPROC) (HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer);
        typedef BOOL (WINAPI * PFNWGLRELEASEVIDEOIMAGENVPROC) (HPBUFFERARB hPbuffer, int iVideoBuffer);
        typedef BOOL (WINAPI * PFNWGLSENDPBUFFERTOVIDEONVPROC) (HPBUFFERARB hPbuffer, int iBufferType, unsigned long *pulCounterPbuffer, BOOL bBlock);
        typedef BOOL (WINAPI * PFNWGLGETVIDEOINFONVPROC) (HPVIDEODEV hpVideoDevice, unsigned long *pulCounterOutputPbuffer, unsigned long *pulCounterOutputVideo);

        #define wglGetVideoDeviceNV      GLEGetCurrentFunction(wglGetVideoDeviceNV)
        #define wglReleaseVideoDeviceNV  GLEGetCurrentFunction(wglReleaseVideoDeviceNV)
        #define wglBindVideoImageNV      GLEGetCurrentFunction(wglBindVideoImageNV)
        #define wglReleaseVideoImageNV   GLEGetCurrentFunction(wglReleaseVideoImageNV)
        #define wglSendPbufferToVideoNV  GLEGetCurrentFunction(wglSendPbufferToVideoNV)
        #define wglGetVideoInfoNV        GLEGetCurrentFunction(wglGetVideoInfoNV)

        #define GLE_WGL_NV_video_output GLEGetCurrentVariable(gl_WGL_NV_video_output)
    #endif


    #ifndef WGL_NV_swap_group
        #define WGL_NV_swap_group 1

        typedef BOOL (WINAPI * PFNWGLJOINSWAPGROUPNVPROC) (HDC hDC, GLuint group);
        typedef BOOL (WINAPI * PFNWGLBINDSWAPBARRIERNVPROC) (GLuint group, GLuint barrier);
        typedef BOOL (WINAPI * PFNWGLQUERYSWAPGROUPNVPROC) (HDC hDC, GLuint *group, GLuint *barrier);
        typedef BOOL (WINAPI * PFNWGLQUERYMAXSWAPGROUPSNVPROC) (HDC hDC, GLuint *maxGroups, GLuint *maxBarriers);
        typedef BOOL (WINAPI * PFNWGLQUERYFRAMECOUNTNVPROC) (HDC hDC, GLuint *count);
        typedef BOOL (WINAPI * PFNWGLRESETFRAMECOUNTNVPROC) (HDC hDC);

        #define wglJoinSwapGroupNV        GLEGetCurrentFunction(wglJoinSwapGroupNV)
        #define wglBindSwapBarrierNV      GLEGetCurrentFunction(wglBindSwapBarrierNV)
        #define wglQuerySwapGroupNV       GLEGetCurrentFunction(wglQuerySwapGroupNV)
        #define wglQueryMaxSwapGroupsNV   GLEGetCurrentFunction(wglQueryMaxSwapGroupsNV)
        #define wglQueryFrameCountNV      GLEGetCurrentFunction(wglQueryFrameCountNV)
        #define wglResetFrameCountNV      GLEGetCurrentFunction(wglResetFrameCountNV)

        #define GLE_WGL_NV_swap_group GLEGetCurrentVariable(gl_WGL_NV_swap_group)
    #endif


    #ifndef WGL_NV_video_capture
        #define WGL_NV_video_capture 1

        #define WGL_UNIQUE_ID_NV               0x20CE
        #define WGL_NUM_VIDEO_CAPTURE_SLOTS_NV 0x20CF

        typedef struct _GPU_DEVICE {
            DWORD  cb;
            CHAR   DeviceName[32];
            CHAR   DeviceString[128];
            DWORD  Flags;
            RECT   rcVirtualScreen;
        } GPU_DEVICE, *PGPU_DEVICE;
        DECLARE_HANDLE(HVIDEOINPUTDEVICENV);

        typedef BOOL (WINAPI * PFNWGLBINDVIDEOCAPTUREDEVICENVPROC) (UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice);
        typedef UINT (WINAPI * PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC) (HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList);
        typedef BOOL (WINAPI * PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC) (HDC hDc, HVIDEOINPUTDEVICENV hDevice);
        typedef BOOL (WINAPI * PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC) (HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int *piValue);
        typedef BOOL (WINAPI * PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC) (HDC hDc, HVIDEOINPUTDEVICENV hDevice);

        #define wglBindVideoCaptureDeviceNV        GLEGetCurrentFunction(wglBindVideoCaptureDeviceNV)
        #define wglEnumerateVideoCaptureDevicesNV  GLEGetCurrentFunction(wglEnumerateVideoCaptureDevicesNV)
        #define wglLockVideoCaptureDeviceNV        GLEGetCurrentFunction(wglLockVideoCaptureDeviceNV)
        #define wglQueryVideoCaptureDeviceNV       GLEGetCurrentFunction(wglQueryVideoCaptureDeviceNV)
        #define wglReleaseVideoCaptureDeviceNV     GLEGetCurrentFunction(wglReleaseVideoCaptureDeviceNV)

        #define GLE_WGL_NV_video_capture GLEGetCurrentVariable(gl_WGL_NV_video_capture)
    #endif


    #ifndef WGL_NV_copy_image
        #define WGL_NV_copy_image 1

        typedef BOOL (WINAPI * PFNWGLCOPYIMAGESUBDATANVPROC) (HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth);

        #define wglCopyImageSubDataNV GLEGetCurrentFunction(wglCopyImageSubDataNV)

        #define GLE_WGL_NV_copy_image GLEGetCurrentVariable(gl_WGL_NV_copy_image)
    #endif


    #ifndef WGL_NV_DX_interop
        #define WGL_NV_DX_interop 1

        #define WGL_ACCESS_READ_ONLY_NV     0x0000
        #define WGL_ACCESS_READ_WRITE_NV    0x0001
        #define WGL_ACCESS_WRITE_DISCARD_NV 0x0002

        typedef BOOL   (WINAPI * PFNWGLDXCLOSEDEVICENVPROC) (HANDLE hDevice);
        typedef BOOL   (WINAPI * PFNWGLDXLOCKOBJECTSNVPROC) (HANDLE hDevice, GLint count, HANDLE* hObjects);
        typedef BOOL   (WINAPI * PFNWGLDXOBJECTACCESSNVPROC) (HANDLE hObject, GLenum access);
        typedef HANDLE (WINAPI * PFNWGLDXOPENDEVICENVPROC) (void* dxDevice);
        typedef HANDLE (WINAPI * PFNWGLDXREGISTEROBJECTNVPROC) (HANDLE hDevice, void* dxObject, GLuint name, GLenum type, GLenum access);
        typedef BOOL   (WINAPI * PFNWGLDXSETRESOURCESHAREHANDLENVPROC) (void* dxObject, HANDLE shareHandle);
        typedef BOOL   (WINAPI * PFNWGLDXUNLOCKOBJECTSNVPROC) (HANDLE hDevice, GLint count, HANDLE* hObjects);
        typedef BOOL   (WINAPI * PFNWGLDXUNREGISTEROBJECTNVPROC) (HANDLE hDevice, HANDLE hObject);

        #define wglDXCloseDeviceNV            GLEGetCurrentFunction(wglDXCloseDeviceNV)
        #define wglDXLockObjectsNV            GLEGetCurrentFunction(wglDXLockObjectsNV)
        #define wglDXObjectAccessNV           GLEGetCurrentFunction(wglDXObjectAccessNV)
        #define wglDXOpenDeviceNV             GLEGetCurrentFunction(wglDXOpenDeviceNV)
        #define wglDXRegisterObjectNV         GLEGetCurrentFunction(wglDXRegisterObjectNV)
        #define wglDXSetResourceShareHandleNV GLEGetCurrentFunction(wglDXSetResourceShareHandleNV)
        #define wglDXUnlockObjectsNV          GLEGetCurrentFunction(wglDXUnlockObjectsNV)
        #define wglDXUnregisterObjectNV       GLEGetCurrentFunction(wglDXUnregisterObjectNV)

        #define GLE_WGL_NV_DX_interop GLEGetCurrentVariable(gl_WGL_NV_DX_interop)
    #endif
    
#endif // GLE_WINDOWS_ENABLED



/************************************************************************************
   Apple-specific (CGL) functionality
************************************************************************************/

#if defined(GLE_APPLE_ENABLED)
    // We don't currently disable Apple's OpenGL/OpenGL.h and replicate its declarations here.
    // We might want to do that if we intended to support hooking its functions here like we do for wgl functions.
    #include <OpenGL/OpenGL.h>
#endif



/************************************************************************************
   Unix-specific (GLX) functionality
************************************************************************************/

#if defined(GLE_UNIX_ENABLED)
    #ifdef __glxext_h_
        #error glxext.h was included before this header. This header needs to be inlcuded instead of or at least before glxext.h
    #endif
    #define __glxext_h_

    #if defined(GLX_H) || defined(__GLX_glx_h__) || defined(__glx_h__)
        #error glx.h was included before this header. This header needs to be inlcuded instead of or at least before glx.h
    #endif
    #define GLX_H
    #define __GLX_glx_h__
    #define __glx_h__

    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xmd.h>

    // GLX version 1.0 functions are assumed to always be present.
    #ifndef GLX_VERSION_1_0
        #define GLX_VERSION_1_0 1

        #define GLX_USE_GL 1
        #define GLX_BUFFER_SIZE 2
        #define GLX_LEVEL 3
        #define GLX_RGBA 4
        #define GLX_DOUBLEBUFFER 5
        #define GLX_STEREO 6
        #define GLX_AUX_BUFFERS 7
        #define GLX_RED_SIZE 8
        #define GLX_GREEN_SIZE 9
        #define GLX_BLUE_SIZE 10
        #define GLX_ALPHA_SIZE 11
        #define GLX_DEPTH_SIZE 12
        #define GLX_STENCIL_SIZE 13
        #define GLX_ACCUM_RED_SIZE 14
        #define GLX_ACCUM_GREEN_SIZE 15
        #define GLX_ACCUM_BLUE_SIZE 16
        #define GLX_ACCUM_ALPHA_SIZE 17
        #define GLX_BAD_SCREEN 1
        #define GLX_BAD_ATTRIBUTE 2
        #define GLX_NO_EXTENSION 3
        #define GLX_BAD_VISUAL 4
        #define GLX_BAD_CONTEXT 5
        #define GLX_BAD_VALUE 6
        #define GLX_BAD_ENUM 7

        typedef XID GLXDrawable;
        typedef XID GLXPixmap;
        typedef unsigned int GLXVideoDeviceNV; 
        typedef struct __GLXcontextRec *GLXContext;

        // GLE_HOOKING_ENABLED
        // We don't currently support hooking the following GLX 1.0 functions like we do with the analagous windows wgl functions.
        // However, we can do this if needed. We would just have something like this:
        //     #define glXQueryExtension(...) GLEGetCurrentFunction(glXQueryExtension)(__VA_ARGS__)
        // plus a member function like:
        //      Bool glXQueryExtension_Hook(Display*, int*, int*);
        // See wglCopyContext for an example.
    
        extern Bool         glXQueryExtension (Display *dpy, int *errorBase, int *eventBase);
        extern Bool         glXQueryVersion (Display *dpy, int *major, int *minor);
        extern int          glXGetConfig (Display *dpy, XVisualInfo *vis, int attrib, int *value);
        extern XVisualInfo* glXChooseVisual (Display *dpy, int screen, int *attribList);
        extern GLXPixmap    glXCreateGLXPixmap (Display *dpy, XVisualInfo *vis, Pixmap pixmap);
        extern void         glXDestroyGLXPixmap (Display *dpy, GLXPixmap pix);
        extern GLXContext   glXCreateContext (Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
        extern void         glXDestroyContext (Display *dpy, GLXContext ctx);
        extern Bool         glXIsDirect (Display *dpy, GLXContext ctx);
        extern void         glXCopyContext (Display *dpy, GLXContext src, GLXContext dst, GLulong mask);
        extern Bool         glXMakeCurrent (Display *dpy, GLXDrawable drawable, GLXContext ctx);
        extern GLXContext   glXGetCurrentContext (void);
        extern GLXDrawable  glXGetCurrentDrawable (void);
        extern void         glXWaitGL (void);
        extern void         glXWaitX (void);
        extern void         glXSwapBuffers (Display *dpy, GLXDrawable drawable);
        extern void         glXUseXFont (Font font, int first, int count, int listBase);

    #endif // GLX_VERSION_1_0



    #ifndef GLX_VERSION_1_1
        #define GLX_VERSION_1_1

        #define GLX_VENDOR 0x1
        #define GLX_VERSION 0x2
        #define GLX_EXTENSIONS 0x3

        // These function pointers are assumed to always be present.
        extern const char* glXQueryExtensionsString (Display *dpy, int screen);
        extern const char* glXGetClientString (Display *dpy, int name);
        extern const char* glXQueryServerString (Display *dpy, int screen, int name);
    #endif


    #ifndef GLX_VERSION_1_2
        #define GLX_VERSION_1_2 1

        typedef Display* (* PFNGLXGETCURRENTDISPLAYPROC) (void);

        #define glXGetCurrentDisplay GLEGetCurrentFunction(glXGetCurrentDisplay)
    #endif



    #ifndef GLX_VERSION_1_3
        #define GLX_VERSION_1_3 1

        #define GLX_FRONT_LEFT_BUFFER_BIT 0x00000001
        #define GLX_RGBA_BIT 0x00000001
        #define GLX_WINDOW_BIT 0x00000001
        #define GLX_COLOR_INDEX_BIT 0x00000002
        #define GLX_FRONT_RIGHT_BUFFER_BIT 0x00000002
        #define GLX_PIXMAP_BIT 0x00000002
        #define GLX_BACK_LEFT_BUFFER_BIT 0x00000004
        #define GLX_PBUFFER_BIT 0x00000004
        #define GLX_BACK_RIGHT_BUFFER_BIT 0x00000008
        #define GLX_AUX_BUFFERS_BIT 0x00000010
        #define GLX_CONFIG_CAVEAT 0x20
        #define GLX_DEPTH_BUFFER_BIT 0x00000020
        #define GLX_X_VISUAL_TYPE 0x22
        #define GLX_TRANSPARENT_TYPE 0x23
        #define GLX_TRANSPARENT_INDEX_VALUE 0x24
        #define GLX_TRANSPARENT_RED_VALUE 0x25
        #define GLX_TRANSPARENT_GREEN_VALUE 0x26
        #define GLX_TRANSPARENT_BLUE_VALUE 0x27
        #define GLX_TRANSPARENT_ALPHA_VALUE 0x28
        #define GLX_STENCIL_BUFFER_BIT 0x00000040
        #define GLX_ACCUM_BUFFER_BIT 0x00000080
        #define GLX_NONE 0x8000
        #define GLX_SLOW_CONFIG 0x8001
        #define GLX_TRUE_COLOR 0x8002
        #define GLX_DIRECT_COLOR 0x8003
        #define GLX_PSEUDO_COLOR 0x8004
        #define GLX_STATIC_COLOR 0x8005
        #define GLX_GRAY_SCALE 0x8006
        #define GLX_STATIC_GRAY 0x8007
        #define GLX_TRANSPARENT_RGB 0x8008
        #define GLX_TRANSPARENT_INDEX 0x8009
        #define GLX_VISUAL_ID 0x800B
        #define GLX_SCREEN 0x800C
        #define GLX_NON_CONFORMANT_CONFIG 0x800D
        #define GLX_DRAWABLE_TYPE 0x8010
        #define GLX_RENDER_TYPE 0x8011
        #define GLX_X_RENDERABLE 0x8012
        #define GLX_FBCONFIG_ID 0x8013
        #define GLX_RGBA_TYPE 0x8014
        #define GLX_COLOR_INDEX_TYPE 0x8015
        #define GLX_MAX_PBUFFER_WIDTH 0x8016
        #define GLX_MAX_PBUFFER_HEIGHT 0x8017
        #define GLX_MAX_PBUFFER_PIXELS 0x8018
        #define GLX_PRESERVED_CONTENTS 0x801B
        #define GLX_LARGEST_PBUFFER 0x801C
        #define GLX_WIDTH 0x801D
        #define GLX_HEIGHT 0x801E
        #define GLX_EVENT_MASK 0x801F
        #define GLX_DAMAGED 0x8020
        #define GLX_SAVED 0x8021
        #define GLX_WINDOW 0x8022
        #define GLX_PBUFFER 0x8023
        #define GLX_PBUFFER_HEIGHT 0x8040
        #define GLX_PBUFFER_WIDTH 0x8041
        #define GLX_PBUFFER_CLOBBER_MASK 0x08000000
        #define GLX_DONT_CARE 0xFFFFFFFF

        typedef XID GLXFBConfigID;
        typedef XID GLXPbuffer;
        typedef XID GLXWindow;
        typedef struct __GLXFBConfigRec *GLXFBConfig;

        typedef struct {
          int event_type; 
          int draw_type; 
          unsigned long serial; 
          Bool send_event; 
          Display *display; 
          GLXDrawable drawable; 
          unsigned int buffer_mask; 
          unsigned int aux_buffer; 
          int x, y; 
          int width, height; 
          int count; 
        } GLXPbufferClobberEvent;
     
        typedef union __GLXEvent {
          GLXPbufferClobberEvent glxpbufferclobber; 
          long pad[24]; 
        } GLXEvent;

        typedef GLXFBConfig* (* PFNGLXCHOOSEFBCONFIGPROC) (::Display *dpy, int screen, const int *attrib_list, int *nelements);
        typedef GLXContext   (* PFNGLXCREATENEWCONTEXTPROC) (::Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
        typedef GLXPbuffer   (* PFNGLXCREATEPBUFFERPROC) (::Display *dpy, GLXFBConfig config, const int *attrib_list);
        typedef GLXPixmap    (* PFNGLXCREATEPIXMAPPROC) (::Display *dpy, GLXFBConfig config, Pixmap pixmap, const int *attrib_list);
        typedef GLXWindow    (* PFNGLXCREATEWINDOWPROC) (::Display *dpy, GLXFBConfig config, Window win, const int *attrib_list);
        typedef void         (* PFNGLXDESTROYPBUFFERPROC) (::Display *dpy, GLXPbuffer pbuf);
        typedef void         (* PFNGLXDESTROYPIXMAPPROC) (::Display *dpy, GLXPixmap pixmap);
        typedef void         (* PFNGLXDESTROYWINDOWPROC) (::Display *dpy, GLXWindow win);
        typedef GLXDrawable  (* PFNGLXGETCURRENTREADDRAWABLEPROC) (void);
        typedef int          (* PFNGLXGETFBCONFIGATTRIBPROC) (::Display *dpy, GLXFBConfig config, int attribute, int *value);
        typedef GLXFBConfig* (* PFNGLXGETFBCONFIGSPROC) (::Display *dpy, int screen, int *nelements);
        typedef void         (* PFNGLXGETSELECTEDEVENTPROC) (::Display *dpy, GLXDrawable draw, unsigned long *event_mask);
        typedef XVisualInfo* (* PFNGLXGETVISUALFROMFBCONFIGPROC) (::Display *dpy, GLXFBConfig config);
        typedef Bool         (* PFNGLXMAKECONTEXTCURRENTPROC) (::Display *display, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
        typedef int          (* PFNGLXQUERYCONTEXTPROC) (::Display *dpy, GLXContext ctx, int attribute, int *value);
        typedef void         (* PFNGLXQUERYDRAWABLEPROC) (::Display *dpy, GLXDrawable draw, int attribute, unsigned int *value);
        typedef void         (* PFNGLXSELECTEVENTPROC) (::Display *dpy, GLXDrawable draw, unsigned long event_mask);
    
        #define glXChooseFBConfig         GLEGetCurrentFunction(glXChooseFBConfig)
        #define glXCreateNewContext       GLEGetCurrentFunction(glXCreateNewContext)
        #define glXCreatePbuffer          GLEGetCurrentFunction(glXCreatePbuffer)
        #define glXCreatePixmap           GLEGetCurrentFunction(glXCreatePixmap)
        #define glXCreateWindow           GLEGetCurrentFunction(glXCreateWindow)
        #define glXDestroyPbuffer         GLEGetCurrentFunction(glXDestroyPbuffer)
        #define glXDestroyPixmap          GLEGetCurrentFunction(glXDestroyPixmap)
        #define glXDestroyWindow          GLEGetCurrentFunction(glXDestroyWindow)
        #define glXGetCurrentReadDrawable GLEGetCurrentFunction(glXGetCurrentReadDrawable)
        #define glXGetFBConfigAttrib      GLEGetCurrentFunction(glXGetFBConfigAttrib)
        #define glXGetFBConfigs           GLEGetCurrentFunction(glXGetFBConfigs)
        #define glXGetSelectedEvent       GLEGetCurrentFunction(glXGetSelectedEvent)
        #define glXGetVisualFromFBConfig  GLEGetCurrentFunction(glXGetVisualFromFBConfig)
        #define glXMakeContextCurrent     GLEGetCurrentFunction(glXMakeContextCurrent)
        #define glXQueryContext           GLEGetCurrentFunction(glXQueryContext)
        #define glXQueryDrawable          GLEGetCurrentFunction(glXQueryDrawable)
        #define glXSelectEvent            GLEGetCurrentFunction(glXSelectEvent)

    #endif // GLX_VERSION_1_3



    #ifndef GLX_VERSION_1_4
        #define GLX_VERSION_1_4 1

        #define GLX_SAMPLE_BUFFERS 100000
        #define GLX_SAMPLES 100001

        // This was glXGetProcAddressARB in GLX versions prior to v1.4.
        // This function pointer is assumed to always be present.
        extern void (* glXGetProcAddress(const GLubyte *procName)) ();
    #endif


    // Note: In order to detect the GLX extensions' availability, we need to call glXQueryExtensionsString instead of glGetString(GL_EXTENSIONS).
    #ifndef GLX_EXT_swap_control
        #define GLX_EXT_swap_control 1

        #define GLX_SWAP_INTERVAL_EXT 0x20F1
        #define GLX_MAX_SWAP_INTERVAL_EXT 0x20F2

        typedef void (* PFNGLXSWAPINTERVALEXTPROC) (Display* dpy, GLXDrawable drawable, int interval);

        #define glXSwapIntervalEXT GLEGetCurrentFunction(glXSwapIntervalEXT)

        #define GLE_GLX_EXT_swap_control GLEGetCurrentVariable(gl_GLX_EXT_swap_control)
    #endif
    

    #ifndef GLX_OML_sync_control
        #define GLX_OML_sync_control 1

        typedef Bool    (* PFNGLXGETMSCRATEOMLPROC) (Display* dpy, GLXDrawable drawable, int32_t* numerator, int32_t* denominator);
        typedef Bool    (* PFNGLXGETSYNCVALUESOMLPROC) (Display* dpy, GLXDrawable drawable, int64_t* ust, int64_t* msc, int64_t* sbc);
        typedef int64_t (* PFNGLXSWAPBUFFERSMSCOMLPROC) (Display* dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder);
        typedef Bool    (* PFNGLXWAITFORMSCOMLPROC) (Display* dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder, int64_t* ust, int64_t* msc, int64_t* sbc);
        typedef Bool    (* PFNGLXWAITFORSBCOMLPROC) (Display* dpy, GLXDrawable drawable, int64_t target_sbc, int64_t* ust, int64_t* msc, int64_t* sbc);

        #define glXGetMscRateOML     GLEGetCurrentFunction(glXGetMscRateOML)
        #define glXGetSyncValuesOML  GLEGetCurrentFunction(glXGetSyncValuesOML)
        #define glXSwapBuffersMscOML GLEGetCurrentFunction(glXSwapBuffersMscOML)
        #define glXWaitForMscOML     GLEGetCurrentFunction(glXWaitForMscOML)
        #define glXWaitForSbcOML     GLEGetCurrentFunction(glXWaitForSbcOML)

        #define GLE_GLX_OML_sync_control GLEGetCurrentVariable(gl_GLX_OML_sync_control)
    #endif

#endif // GLE_UNIX_ENABLED


// Undo some defines, because the user may include <Windows.h> after including this header.
#if defined(GLE_WINGDIAPI_DEFINED)
    #undef WINGDIAPI
#endif


#ifdef __cplusplus
} // extern "C"
#endif



#endif // Header include guard





