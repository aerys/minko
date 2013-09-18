package aerys.minko.render.shader.background
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	
	public class BackgroundLayerShader extends Shader
	{
		private static const ZMAX	: Number	= 1. - 1e-7;
        
        private var _diffuse    : DiffuseShaderPart;

		public function BackgroundLayerShader(target : RenderTarget = null)
		{
			super(target, 0.);
            
            _diffuse = new DiffuseShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			return float4(multiply(vertexXYZ.xy, 2), ZMAX, 1);
		}
		
		override protected function getPixelColor() : SFloat
		{
            var uv : SFloat = interpolate(float2(
		        divide(add(multiply(vertexXYZ.x, 2), 1), 2),
		        divide(add(multiply(vertexXYZ.x, 2), 1), 2)
            ));
            
            return _diffuse.getDiffuseColor(false, interpolate(vertexUV));
		}
	}
}
