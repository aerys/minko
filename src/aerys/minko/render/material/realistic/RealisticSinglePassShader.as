package aerys.minko.render.material.realistic
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.material.environment.EnvironmentMappingProperties;
    import aerys.minko.render.material.phong.PhongSinglePassShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.part.BlendingShaderPart;
    import aerys.minko.render.shader.part.DiffuseShaderPart;
    import aerys.minko.render.shader.part.environment.EnvironmentMappingShaderPart;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.Blending;
    import aerys.minko.type.enum.BlendingDestination;
    import aerys.minko.type.enum.BlendingSource;
    
    public class RealisticSinglePassShader extends BasicShader
    {
        private var _diffuse            : DiffuseShaderPart;
        private var _phong              : PhongShaderPart;
        private var _blending           : BlendingShaderPart;
        private var _environmentMapping : EnvironmentMappingShaderPart;
        
        public function RealisticSinglePassShader(target      : RenderTarget  = null,
                                                  priority    : Number        = 0.)
        {
            super(target, priority);
         
            _diffuse = new DiffuseShaderPart(this);
            _phong = new PhongShaderPart(this);
            _blending = new BlendingShaderPart(this);
            _environmentMapping = new EnvironmentMappingShaderPart(this);
        }
        
        override protected function getPixelColor() : SFloat
        {
            var diffuse         : SFloat    = _diffuse.getDiffuseColor();
            var alpha           : SFloat    = diffuse.a;
            var phong           : SFloat    = add(_phong.getDynamicLighting(), _phong.getStaticLighting());
            var env             : SFloat    = _environmentMapping.getEnvironmentColor();
            var envBlending     : uint      = meshBindings.getProperty(
                EnvironmentMappingProperties.ENVIRONMENT_BLENDING, Blending.ALPHA
            );
            
            diffuse = _blending.blend(env, diffuse, envBlending);
            
            return float4(multiply(diffuse.rgb, phong.rgb), alpha);
        }       
    }
}