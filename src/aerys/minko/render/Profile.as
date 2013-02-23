package aerys.minko.render
{
    public final class Profile
    {
        public static const BASELINE   : Profile   = new Profile(200, 128, 8, 8, 8, 200, 28, 8, 8);
        
        private var _numMaxVertexOperations            : uint;
        private var _numMaxVertexConstants             : uint;
        private var _numMaxVertexTemporaries           : uint;
        private var _numMaxVertexAttributes            : uint;
        
        private var _numMaxVaryings                    : uint;
        
        private var _numMaxFragmentOperations          : uint;
        private var _numMaxFragmentConstants           : uint;
        private var _numMaxFragmentTemporaries         : uint;
        private var _numMaxFragmentTextureSamplers     : uint;
        
        public function get numMaxVertexOperations() : uint
        {
            return _numMaxVertexOperations;
        }
        
        public function get numMaxVertexConstants() : uint
        {
            return _numMaxVertexConstants;
        }
        
        public function get numMaxVertexTemporaries() : uint
        {
            return _numMaxVertexTemporaries;
        }
        
        public function get numMaxVertexAttributes() : uint
        {
            return _numMaxVertexAttributes;
        }
        
        public function get numMaxVaryings() : uint
        {
            return _numMaxVaryings;
        }
        
        public function get numMaxFragmentOperations() : uint
        {
            return _numMaxFragmentOperations;
        }
        
        public function get numMaxFragmentConstants() : uint
        {
            return _numMaxFragmentConstants;
        }
        
        public function get numMaxFragmentTemporaries() : uint
        {
            return _numMaxFragmentTemporaries;
        }
        
        public function get numMaxFragmentSamplers() : uint
        {
            return _numMaxFragmentTextureSamplers;
        }
        
        public function Profile(numMaxVertexOperations         : uint,
                                numMaxVertexConstants          : uint,
                                numMaxVertexTemporaries        : uint,
                                numMaxVertexAttributes         : uint,
                                numMaxVaryings                 : uint,
                                numMaxFragmentOperations       : uint,
                                numMaxFragmentConstants        : uint,
                                numMaxFragmentTemporaries      : uint,
                                numMaxFragmentSamplers         : uint)
        {
            _numMaxVertexOperations = numMaxVertexOperations;
            _numMaxVertexConstants = numMaxVertexConstants;
            _numMaxVertexTemporaries = numMaxVertexTemporaries;
            _numMaxVertexAttributes = numMaxVertexAttributes;
            
            _numMaxVaryings = numMaxVaryings;
            
            _numMaxFragmentOperations = numMaxFragmentOperations;
            _numMaxFragmentConstants = numMaxFragmentConstants;
            _numMaxFragmentTemporaries = numMaxFragmentTemporaries;
            _numMaxFragmentTextureSamplers = numMaxFragmentSamplers;
        }
    }
}