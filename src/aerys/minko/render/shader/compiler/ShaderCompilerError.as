package aerys.minko.render.shader.compiler
{
    public final class ShaderCompilerError extends Error
    {
        public static const TOO_MANY_VERTEX_OPERATIONS            : uint  = 1 << 0;
        public static const TOO_MANY_VERTEX_CONSTANTS             : uint  = 1 << 1;
        public static const TOO_MANY_VERTEX_TEMPORARIES           : uint  = 1 << 2;
        public static const TOO_MANY_VERTEX_ATTRIBUTES            : uint  = 1 << 3;
        
        public static const TOO_MANY_VARYINGS                     : uint  = 1 << 4;
        
        public static const TOO_MANY_FRAGMENT_OPERATIONS          : uint  = 1 << 5;
        public static const TOO_MANY_FRAGMENT_CONSTANTS           : uint  = 1 << 6;
        public static const TOO_MANY_FRAGMENT_TEMPORARIES         : uint  = 1 << 7;
        
        public static const TOO_MANY_FRAGMENT_SAMPLERS    : uint  = 1 << 8;
               
        public static const OUT_OF_REGISTERS                      : uint  =
              TOO_MANY_VERTEX_CONSTANTS
            | TOO_MANY_VERTEX_TEMPORARIES
            | TOO_MANY_VERTEX_ATTRIBUTES
            | TOO_MANY_FRAGMENT_CONSTANTS
            | TOO_MANY_FRAGMENT_TEMPORARIES
            | TOO_MANY_FRAGMENT_SAMPLERS;
        
        public function ShaderCompilerError(id : uint = 0)
        {
            super(message, id);
        }
    }
}