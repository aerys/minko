package aerys.minko.render.material.phong
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.phong.LightAwareDiffuseShaderPart;
	import aerys.minko.render.shader.part.phong.PhongShaderPart;
	
	public class PhongShader extends BasicShader
	{
		private var _diffuse	: LightAwareDiffuseShaderPart;
		private var _phong		: PhongShaderPart;
		private var _shadowMap	: ITextureResource;
		
		public function get shadowMap():ITextureResource
		{
			return _shadowMap;
		}

		public function set shadowMap(value:ITextureResource):void
		{
			_shadowMap = value;
		}
		
		public function PhongShader(renderTarget	: RenderTarget		= null,
									priority		: Number			= 0.,
									shadowMap		: ITextureResource	= null)
		{
			super(renderTarget, priority);
			
			// init shader parts
			_shadowMap	= shadowMap;
			_diffuse	= new LightAwareDiffuseShaderPart(this);
			_phong		= new PhongShaderPart(this);
		}

		override protected function getPixelColor() : SFloat
		{
			var color	 : SFloat = _diffuse.getDiffuseColor();
			var lighting : SFloat = _phong.getLightingColor(_shadowMap);
			
			color = float4(multiply(lighting, color.rgb), color.a);
			
			return color;
		}
	}
}
