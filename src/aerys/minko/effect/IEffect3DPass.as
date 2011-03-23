package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public interface IEffect3DPass
	{
		function begin(renderer : IRenderer3D, style : StyleStack3D) : Boolean;
		function end(renderer : IRenderer3D, style : StyleStack3D) : void;
	}
}