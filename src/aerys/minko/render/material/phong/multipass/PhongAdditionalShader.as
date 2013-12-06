package aerys.minko.render.material.phong.multipass
{
    import aerys.minko.render.RenderTarget;
    import aerys.minko.render.material.basic.BasicShader;
    import aerys.minko.render.shader.SFloat;
    import aerys.minko.render.shader.ShaderOptimization;
    import aerys.minko.render.shader.ShaderSettings;
    import aerys.minko.render.shader.part.phong.PhongShaderPart;
    import aerys.minko.type.enum.Blending;
    import aerys.minko.type.enum.DepthTest;
    
    public class PhongAdditionalShader extends BasicShader
    {
        private var _phong		: PhongShaderPart;
        
        private var _lightId    : int;
        private var _diffuse    : Boolean;
        private var _specular   : Boolean;
		
        public function PhongAdditionalShader(lightId        : int,
                                              diffuse        : Boolean,
                                              specular       : Boolean,
                                              renderTarget   : RenderTarget  = null,
                                              priority       : Number        = 0.0)
        {
			super(renderTarget, priority);
            
			optimization |= ShaderOptimization.RESOLVED_PARAMETRIZATION;
			
            _phong = new PhongShaderPart(this);
            
            _lightId = lightId;
            _diffuse = diffuse;
            _specular = specular;
        }
        
        override protected function initializeSettings(settings : ShaderSettings) : void
        {           
			super.initializeSettings(settings);			
			
			settings.blending 	= Blending.ADDITIVE;
			settings.depthTest 	= DepthTest.LESS | DepthTest.EQUAL;
			
			//settings.priority = _priority;
        }
        
        override protected function getPixelColor() : SFloat
        {
			var materialDiffuse	: SFloat	= diffuse.getDiffuseColor();
			var shading			: SFloat	= _phong.getDynamicLighting(_lightId, false, _diffuse, _specular, materialDiffuse);
			
            return float4(shading.rgb, 1);
        }
    }
}