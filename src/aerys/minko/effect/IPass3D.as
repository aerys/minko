package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public interface IPass3D
	{
		function begin(renderer : IRenderer3D) : void;
		function end(renderer : IRenderer3D) : void;
	}
}