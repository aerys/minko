package aerys.minko.render.effect.debug
{
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.type.math.Vector4;

	public class DebugEffect extends SinglePassEffect
	{
		private static const COLOR	: Vector4	= new Vector4(.5, .5, .5, 1.);
		
		private var _vertexColor	: SValue	= null;
		
		override protected function getOutputPosition() : SValue
		{
			var lightDir	: SValue	= cameraLocalDirection;
			var normal		: SValue	= vertexNormal;
			
			_vertexColor = saturate(negate(normal.dotProduct3(lightDir)));
			_vertexColor.scaleBy(COLOR);
			
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			return interpolate(_vertexColor);
		}
		
	}
}