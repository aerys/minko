package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public interface IEffect3DPass
	{
		function begin(renderer : IRenderer3D, style : Effect3DStyle) : Boolean;
		function end(renderer : IRenderer3D, style : Effect3DStyle) : void;
	}
}