package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public interface IEffect3D extends IStyled3D
	{
		function get passes() : Vector.<IEffect3DPass>;
		
		function begin(renderer : IRenderer3D, style : StyleStack3D) : void;
		function end(renderer : IRenderer3D, style : StyleStack3D) : void;
	}
}
