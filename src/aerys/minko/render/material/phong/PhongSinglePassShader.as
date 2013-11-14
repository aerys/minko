package aerys.minko.render.material.phong
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderOptimization;
	import aerys.minko.render.shader.part.phong.LightAwareDiffuseShaderPart;
	import aerys.minko.render.shader.part.phong.PhongShaderPart;
	
	public class PhongSinglePassShader extends BasicShader
	{
		private var _diffuse	: LightAwareDiffuseShaderPart;
		private var _phong		: PhongShaderPart;

		public function PhongSinglePassShader(renderTarget	: RenderTarget		= null,
                                              priority		: Number			= 0.)
		{
			super(renderTarget, priority);
			
			optimization |= ShaderOptimization.RESOLVED_PARAMETRIZATION;
			
			// init shader parts
			_diffuse	= new LightAwareDiffuseShaderPart(this);
			_phong		= new PhongShaderPart(this);
		}

		override protected function getPixelColor() : SFloat
		{
            var materialDiffuse : SFloat = _diffuse.getDiffuseColor();
			var shading			: SFloat = add(
				_phong.getStaticLighting(float4(1, 1, 1, 1)),
				_phong.getDynamicLighting(-1, true, true, true, materialDiffuse)
			);    
			
			return float4(shading.rgb, materialDiffuse.a);
		}
	}
}
