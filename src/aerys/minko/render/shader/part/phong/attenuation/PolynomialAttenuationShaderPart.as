package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	
	public final class PolynomialAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		public function PolynomialAttenuationShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			var polynomial			: SFloat	= getLightParameter(
				lightId, 'attenuationPolynomial', 3
			);
			var lightWorldPosition	: SFloat 	= getLightParameter(lightId, 'worldPosition', 3);
			var lightToPoint		: SFloat 	= subtract(fsWorldPosition, lightWorldPosition);
			var squareDistance		: SFloat 	= dotProduct3(lightToPoint, lightToPoint);
			var distance			: SFloat	= sqrt(squareDistance);
			var constant			: SFloat	= polynomial.x;
			var linear				: SFloat	= polynomial.y;
			var quadratic			: SFloat	= polynomial.z;
			
			return saturate(divide(
				1.,
				add(constant, multiply(distance, linear), multiply(squareDistance, quadratic))
			));
		}
	}
}