package aerys.minko.render.shader.part.depth
{
	import aerys.minko.render.shader.SFloat;

	public interface IDepthShaderPart
	{
		function getVertexPosition(lightId : uint, vPosition : SFloat, face : uint = 4) : SFloat;
		function getPixelColor(lightId : uint) : SFloat;
		function getUV(lightId : uint, worldPosition : SFloat) : SFloat;
		function getDepthForAttenuation(lightId : uint, worldPosition : SFloat) : SFloat;
	}
}