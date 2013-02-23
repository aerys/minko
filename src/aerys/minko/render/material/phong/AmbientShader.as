package aerys.minko.render.material.phong
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.Blending;
    
    public class AmbientShader extends BasicShader
    {
        private var _phong : PhongShaderPart;
        
        public function AmbientShader(renderTarget  : RenderTarget  = null,
                                      priority      : Number        = 0.0)
        {
            super(renderTarget, priority);
            
            _phong = new PhongShaderPart(this);
        }
        
        override protected function initializeSettings(settings:ShaderSettings):void
        {
            super.initializeSettings(settings);
            
            settings.blending = Blending.OPAQUE;
        }
        
        override protected function getPixelColor() : SFloat
        {
            var diffuse : SFloat    = super.getPixelColor();
            
            return float4(multiply(diffuse.rgb, _phong.getAmbientLighting()), diffuse.a);
        }
    }
}