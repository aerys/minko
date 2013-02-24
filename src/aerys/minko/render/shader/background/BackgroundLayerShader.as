package aerys.minko.render.shader.background
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	
	public class BackgroundLayerShader extends BasicShader
	{
		private static const ZMAX	: Number	= 1. - 1e-7;
        
        private var _diffuse    : DiffuseShaderPart;
        private var _uv         : SFloat;
        
		public function BackgroundLayerShader(target : RenderTarget = null)
		{
			super(target, 0.);
            
            _diffuse = new DiffuseShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			return float4(sign(vertexXYZ.xy), ZMAX, 1);
		}
		
		override protected function getPixelColor() : SFloat
		{
            var uv : SFloat = interpolate(divide(add(sign(vertexXYZ.xy), 1), 2));
            
            return _diffuse.getDiffuseColor(false, uv);
		}
	}
}
