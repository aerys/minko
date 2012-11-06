package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	
	public class DistanceAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		public function DistanceAttenuationShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			var lightWorldPosition	: SFloat = getLightParameter(lightId, 'worldPosition', 3);
			var lightDistance 		: SFloat = getLightParameter(lightId, 'attenuationDistance', 1);
			
			var lightSquareDistance	: SFloat = multiply(lightDistance, lightDistance);
			var lightToPoint		: SFloat = subtract(fsWorldPosition, lightWorldPosition);
			var squareDistance		: SFloat = dotProduct3(lightToPoint, lightToPoint);
			
			return saturate(divide(lightSquareDistance, squareDistance));
		}
	}
}
