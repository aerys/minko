package aerys.minko.render.material.phong
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
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
			
			// init shader parts
			_diffuse	= new LightAwareDiffuseShaderPart(this);
			_phong		= new PhongShaderPart(this);
		}

		override protected function getPixelColor() : SFloat
		{
            var diffuse : SFloat = _diffuse.getDiffuseColor();
            var phong   : SFloat = add(_phong.getDynamicLighting(), _phong.getStaticLighting());    
            
            return float4(multiply(diffuse.rgb, phong), diffuse.a);
		}
	}
}
