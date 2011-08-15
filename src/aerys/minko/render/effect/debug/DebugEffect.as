package aerys.minko.render.effect.debug
{
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.type.math.Vector4;

	public class DebugEffect extends SinglePassEffect
	{
		private const COLOR			: SValue	= float4(.5, .5, .5, 1.);
		
		private var _vertexColor	: SValue	= null;
		
		override protected function getOutputPosition() : SValue
		{
			var lightDir	: SValue	= subtract(vertexPosition, cameraLocalPosition);
			
			lightDir.normalize();
			
			_vertexColor = vertexNormal.dotProduct3(lightDir);
			_vertexColor = float4(multiply(_vertexColor, COLOR.rgb), COLOR.a);
			
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			return interpolate(_vertexColor);
		}
		
	}
}