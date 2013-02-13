package aerys.minko.render.effect.blur
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.PostProcessingShaderPart;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public class BlurShader extends Shader
	{
		public static const DIRECTION_VERTICAL		: uint	= 0;
		public static const DIRECTION_HORIZONTAL	: uint	= 1;
		
		private var _direction		: uint						= 0;
		
		private var _postProcessing	: PostProcessingShaderPart	= null;
		private var _blur			: BlurShaderPart			= null;
		
		private var _blurSource		: ITextureResource			= null;
		
		protected function get postProcessing() : PostProcessingShaderPart
		{
			return _postProcessing;
		}
		
		protected function get blurSourceTexture() : SFloat
		{
			var source : SFloat	= _postProcessing.backBufferTexture;
			
			if (_blurSource != null)
			{
				source = getTexture(
					_blurSource,
					SamplerFiltering.LINEAR,
					SamplerMipMapping.LINEAR,
					SamplerWrapping.CLAMP
				);
			}
			
			return source;
		}
		
		public function BlurShader(direction	: uint,
								   blurSource	: ITextureResource	= null,
								   renderTarget	: RenderTarget 		= null,
								   priority		: Number			= 0.0)
		{
			super(renderTarget, priority);
			
			_direction = direction;
			_blurSource = blurSource;
			
			_postProcessing = new PostProcessingShaderPart(this);
			_blur = new BlurShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			return _postProcessing.vertexPosition;
		}
		
		override protected function getPixelColor() : SFloat
		{
			var size    : SFloat    = _blurSource
				? float2(_blurSource.width, _blurSource.height)
				: float2(viewportWidth, viewportHeight);
            var spread  : SFloat    = sceneBindings.propertyExists('blurSpread')
                ? sceneBindings.getParameter('blurSpread', 2)
                : null;
			
			if (_direction == DIRECTION_HORIZONTAL)
				return _blur.linearGaussianBlurX(blurSourceTexture, size, spread ? spread.x : null);
			else
				return _blur.linearGaussianBlurY(blurSourceTexture, size, spread ? spread.y : null);
		}
	}
}