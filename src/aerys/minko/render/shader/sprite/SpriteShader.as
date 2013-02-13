package aerys.minko.render.shader.sprite
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.part.DiffuseShaderPart;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public class SpriteShader extends Shader
	{
        private var _diffuse    : DiffuseShaderPart;
        
		private var _uv	        : SFloat;
		
		public function SpriteShader()
		{
			super();
            
            _diffuse = new DiffuseShaderPart(this);
		}
		
		override protected function initializeSettings(settings : ShaderSettings) : void
		{
//			settings.blending = Blending.ALPHA;
			settings.depthTest = DepthTest.LESS;
		}
		
		override protected function getVertexPosition() : SFloat
		{
			var vertexXY	: SFloat	= vertexXY.xy;
		
			_uv = multiply(add(vertexXY, 0.5), float2(1, -1));
			
			var depth	: SFloat 	= meshBindings.getParameter('depth', 1);
			
			var xy		: SFloat	= float2(
				meshBindings.getParameter('x', 1),
				meshBindings.getParameter('y', 1)
			);
			var size	: SFloat	= float2(
				meshBindings.getParameter('width', 1),
				meshBindings.getParameter('height', 1)
			);
			var vpSize	: SFloat	= float2(
				sceneBindings.getParameter('viewportWidth', 1),
				sceneBindings.getParameter('viewportHeight', 1)
			);
			
			xy = divide(add(xy, divide(size, 2)), vpSize);
			xy = multiply(subtract(xy, 0.5), float2(2, -2));
			
			vertexXY.scaleBy(divide(size, divide(vpSize, 2)));
			vertexXY.incrementBy(xy);
			
			return float4(vertexXY, depth, 1);
		}
		
		override protected function getPixelColor() : SFloat
		{
			return _diffuse.getDiffuseColor(true, _uv);
		}
	}
}