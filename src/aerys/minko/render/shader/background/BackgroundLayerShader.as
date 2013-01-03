package aerys.minko.render.shader.background
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class BackgroundLayerShader extends BasicShader
	{
		public function BackgroundLayerShader(target : RenderTarget = null)
		{
			super(target, Number.MAX_VALUE);
		}
		private static const ZMAX	: Number	= 1. - 1e-7;
		
		override protected function getVertexPosition() : SFloat
		{
			return float4(
				multiply(vertexXYZ.x, 2),
				multiply(vertexXYZ.y, 2),
				ZMAX,
				1
			);
		}
		
		override protected function getPixelColor() : SFloat
		{
			var diffuseMap	: SFloat	= meshBindings.getTextureParameter(BasicProperties.DIFFUSE_MAP);
			
			var uv 			: SFloat 	= multiply(
				float4(1, 1, 1, 1),
				interpolate(vertexUV)
			);
			
			return sampleTexture(diffuseMap, uv);
		}
	}
}
