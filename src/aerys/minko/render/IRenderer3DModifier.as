package aerys.minko.render
{
	public interface IRenderer3DModifier extends IRenderer3D
	{
		function get target() : IRenderer3D;
		function set target(value : IRenderer3D) : void;
	}
}