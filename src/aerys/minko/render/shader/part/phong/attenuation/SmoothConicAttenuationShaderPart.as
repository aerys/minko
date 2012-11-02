package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	
	public class SmoothConicAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		public function SmoothConicAttenuationShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			// retrieve light data.
			var lightWorldPosition		: SFloat = getLightParameter(lightId, 'worldPosition', 3);
			var lightWorldDirection		: SFloat = getLightParameter(lightId, 'worldDirection', 3);
			
			// compute cone (constant) factors. This will be resolved by the compiler.
			var halfInnerRadius			: SFloat = getLightParameter(lightId, 'innerRadius', 1).scaleBy(1 / 2);
			var halfOuterRadius			: SFloat = getLightParameter(lightId, 'outerRadius', 1).scaleBy(1 / 2);
			var factor1					: SFloat = divide(1, subtract(cos(halfInnerRadius), cos(halfOuterRadius)));
			var factor2					: SFloat = subtract(1, multiply(cos(halfInnerRadius), factor1));
			
			// compute attenuation factor
			var lightToPoint			: SFloat = subtract(fsWorldPosition, lightWorldPosition);
			var lightAngleCosine		: SFloat = dotProduct3(lightWorldDirection, normalize(lightToPoint));
			
			return saturate(add(multiply(factor1, lightAngleCosine), factor2));
		}
	}
}
