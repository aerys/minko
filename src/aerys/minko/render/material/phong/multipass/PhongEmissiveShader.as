package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.Blending;
    import aerys.minko.type.enum.BlendingDestination;
    import aerys.minko.type.enum.BlendingSource;
    import aerys.minko.type.enum.DepthTest;
    
    public class PhongEmissiveShader extends BasicShader
    {
        public function PhongEmissiveShader(renderTarget  : RenderTarget  = null,
                                            priority      : Number        = 0.0)
        {
            super(renderTarget, priority);
        }
        
        override protected function initializeSettings(settings : ShaderSettings) : void
        {
            super.initializeSettings(settings);
            
            settings.blending = BlendingDestination.SOURCE_COLOR | BlendingSource.ZERO;
            settings.depthTest = DepthTest.LESS | DepthTest.EQUAL;
        }
        
        override protected function getPixelColor() : SFloat
        {
            return super.getPixelColor();
        }
    }
}