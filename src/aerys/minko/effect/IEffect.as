package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;

	public interface IEffect extends IStyled
	{
		function get passes() : Vector.<IEffectPass>;
		
		function begin(renderer : IRenderer, style : StyleStack) : void;
		function end(renderer : IRenderer, style : StyleStack) : void;
	}
}
