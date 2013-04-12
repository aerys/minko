package aerys.minko.render.shader.vertex
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.Shader;
    
    public class VertexDepthShader extends Shader
    {
        private var _vertexPosition	: SFloat;
        
        public function VertexDepthShader(target	: RenderTarget	= null,
                                          priority	: Number		= 0.)
        {
            super(target, priority);
        }
        
        override protected function getVertexPosition() : SFloat
        {
            _vertexPosition = localToScreen(vertexXYZ);
            
            return _vertexPosition;
        }
        
        override protected function getPixelColor() : SFloat
        {
            var depth    : SFloat   = interpolate(divide(
                _vertexPosition.z,
                subtract(cameraZFar, cameraZNear)
            ));
            
            return pack(depth);
        }
    }
}