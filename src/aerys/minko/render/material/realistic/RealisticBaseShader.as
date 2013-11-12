package aerys.minko.render.material.realistic
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderOptimization;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.environment.EnvironmentMappingShaderPart;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    
    public class RealisticBaseShader extends BasicShader
    {
        private var _phong      : PhongShaderPart;
        private var _envMapping	: EnvironmentMappingShaderPart;
        
        public function RealisticBaseShader(target      : RenderTarget  = null,
                                            priority    : Number        = 0.)
        {
            super(target, priority);
            
			optimization |= ShaderOptimization.RESOLVED_PARAMETRIZATION;
			
            _phong = new PhongShaderPart(this);
            _envMapping = new EnvironmentMappingShaderPart(this);
        }
        
        override protected function getPixelColor() : SFloat
        {
//            var diffuse     : SFloat    = _envMapping.applyEnvironmentMapping(super.getPixelColor());
//            var lighting    : SFloat    = _phong.getBaseLighting();
//            
//            return float4(
//                multiply(diffuse.rgb, lighting),
//                diffuse.a
//            );
            
            return float4(1, 1, 1, 1);
        }        
    }
}