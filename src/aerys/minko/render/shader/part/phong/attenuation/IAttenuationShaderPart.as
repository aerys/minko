package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;

	public interface IAttenuationShaderPart
	{
		function getAttenuation(lightId : uint) : SFloat
	}
}
