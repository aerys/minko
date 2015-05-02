#if __VERSION__ < 110
    #extension GL_OES_standard_derivatives : enable
#endif

#if __VERSION__ < 130
    #if GL_ES
        #extension GL_EXT_shader_texture_lod : enable
    #elif __VERSION__ > 100
        #extension GL_ARB_shader_texture_lod : enable
    #endif
#endif
