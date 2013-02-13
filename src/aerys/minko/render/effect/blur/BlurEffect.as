package aerys.minko.render.effect.blur
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;

	/**
	 * The BlurEffect is a multi-pass linear gaussian blur post-processing effect.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class BlurEffect extends Effect
	{
		/**
		 * 
		 * @param quality
		 * @param numPasses
		 * 
		 */
		public function BlurEffect(quality : uint, numPasses : uint = 1)
		{
			super();
			
			setExtraPasses(getBlurPasses(quality, numPasses));
		}
		
		public static function getBlurPasses(quality 		: uint,
											 numPasses 		: uint,
											 aspectRatio	: Number			= 1.,
											 blurSource		: ITextureResource	= null,
											 blurTarget		: RenderTarget		= null,
											 priorityOffset	: Number			= 0.,
											 passes			: Vector.<Shader>	= null) : Vector.<Shader>
		{
			passes = passes != null ? passes : new <Shader>[];
			
			var numHorizontalPasses	: uint			= Math.ceil((aspectRatio * numPasses) / (1 + aspectRatio));
			var numVerticalPasses	: uint			= numPasses - numHorizontalPasses;
			var target1				: RenderTarget	= new RenderTarget(
				quality, quality, new TextureResource(quality, quality)
			);
			var target2				: RenderTarget	= new RenderTarget(
				quality, quality, new TextureResource(quality, quality)
			);
			
			for (var i : uint = 0; i < numPasses; ++i)
			{
				var passTarget		: RenderTarget 		= null;
				var passSource		: ITextureResource	= null;
				var passDirection	: uint				= 0;
				
				if (i % 2 == 0)
				{
					passTarget = target1;
					passSource = target2.textureResource;
					
					if (numHorizontalPasses != 0)
					{
						--numHorizontalPasses;
						passDirection = BlurShader.DIRECTION_HORIZONTAL;
					}
					else
					{
						passDirection = BlurShader.DIRECTION_VERTICAL;
					}
				}
				else
				{
					passTarget = target2;
					passSource = target1.textureResource;
					
					if (numVerticalPasses != 0)
					{
						--numVerticalPasses;
						passDirection = BlurShader.DIRECTION_VERTICAL;
					}
					else
					{
						passDirection = BlurShader.DIRECTION_HORIZONTAL;
					}
				}
				
				passes.push(new BlurShader(
					passDirection,
					i == 0 ? blurSource : passSource,
					i == numPasses - 1 ? blurTarget : passTarget,
					priorityOffset - (i / numPasses)
				));
			}
			
			return passes;
		}
	}
}