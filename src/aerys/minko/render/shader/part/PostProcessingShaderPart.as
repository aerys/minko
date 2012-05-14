package aerys.minko.render.shader.part
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public final class PostProcessingShaderPart extends ShaderPart
	{
		public function get backBufferTexture() : SFloat
		{
			return sceneBindings.getTextureParameter(
				"backBuffer",
				SamplerFiltering.LINEAR,
				SamplerMipMapping.DISABLE,
				SamplerWrapping.CLAMP
			);
		}
		
		public function get vertexPosition() : SFloat
		{
			return multiply(vertexXYZ, float4(1, 1, 1, .5));
		}
		
		public function get backBufferPixel() : SFloat
		{
			return sampleBackBuffer(interpolate(vertexUV));
		}
		
		public function PostProcessingShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function initializeSettings(settings : ShaderSettings) : void
		{
			settings.depthWriteEnabled = false;
			settings.depthTest = DepthTest.ALWAYS;
		}
		
		public function sampleBackBuffer(uv : SFloat) : SFloat
		{
			return sampleTexture(backBufferTexture, interpolate(vertexUV));
		}
	}
}