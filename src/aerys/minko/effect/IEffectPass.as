package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;

	public interface IEffectPass
	{
		function begin(renderer : IRenderer, style : StyleStack) : Boolean;
		function end(renderer : IRenderer, style : StyleStack) : void;
	}
}