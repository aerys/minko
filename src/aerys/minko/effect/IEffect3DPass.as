package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public interface IEffect3DPass
	{
		function begin(renderer : IRenderer3D, style : Object = null) : void;
		function end(renderer : IRenderer3D, style : Object = null) : void;
	}
}