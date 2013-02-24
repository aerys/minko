package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.Shader;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.DepthTest;
    
    public class PhongAmbientShader extends Shader
    {
        private var _phong  : PhongShaderPart;
        
        public function PhongAmbientShader(renderTarget : RenderTarget  = null,
                                           priority     : Number        = 0.0)
        {
            super(renderTarget, priority);
            
            _phong = new PhongShaderPart(this);
        }
        
        override protected function initializeSettings(settings : ShaderSettings) : void
        {
            super.initializeSettings(settings);
            
            settings.depthTest = DepthTest.LESS | DepthTest.EQUAL;
        }
        
        override protected function getVertexPosition() : SFloat
        {
            return localToScreen(vertexXYZ);
        }
        
        override protected function getPixelColor() : SFloat
        {
            return float4(add(_phong.getAmbientLighting(), _phong.getStaticLighting()), 1);
        }
    }
}