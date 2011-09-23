package aerys.minko.render.effect.debug
{
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.shader.SValue;

	public class VertexNormalColorEffect extends SinglePassEffect implements IRenderingEffect
	{
		override protected function getOutputPosition() : SValue
		{
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			return divide(add(1., interpolate(vertexNormal)), 2.);
		}
	}
}