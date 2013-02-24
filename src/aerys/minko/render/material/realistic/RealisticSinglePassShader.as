package aerys.minko.render.material.realistic
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.part.environment.EnvironmentMappingShaderPart;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    
    public class RealisticSinglePassShader extends BasicShader
    {
        private var _phong      : PhongShaderPart;
        private var _envMapping	: EnvironmentMappingShaderPart;
        
        public function RealisticSinglePassShader(target      : RenderTarget  = null,
                                                  priority    : Number        = 0.)
        {
            super(target, priority);
            
            _phong = new PhongShaderPart(this);
            _envMapping = new EnvironmentMappingShaderPart(this);
        }
        
        override protected function getPixelColor() : SFloat
        {
            var diffuse     : SFloat    = _envMapping.applyEnvironmentMapping(super.getPixelColor());
            
            return float4(
                add(
                    multiply(diffuse.rgb, _phong.getBaseLighting()),
                    _phong.getAdditionalLighting()
                ),
                diffuse.a
            );
        }       
    }
}