package aerys.minko.render.shader.part
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.SamplerFilter;
	import aerys.minko.type.enum.SamplerMipmap;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public final class PostProcessingShaderPart extends ShaderPart
	{
		public function PostProcessingShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function initializeSettings(settings : ShaderSettings) : void
		{
			settings.enableDepthWrite = false;
			settings.depthTest = DepthTest.ALWAYS;
		}
		
		public function get vertexPosition() : SFloat
		{
			return multiply(vertexXYZ, float4(1, 1, 1, .5));
		}
		
		public function get backBufferPixel() : SFloat
		{
			var backBuffer	: SFloat	= sceneBindings.getTextureParameter(
				"backBuffer",
				SamplerFilter.LINEAR,
				SamplerMipmap.DISABLE,
				SamplerWrapping.CLAMP
			);
			
			return sampleTexture(backBuffer, interpolate(vertexUV));
		}
	}
}