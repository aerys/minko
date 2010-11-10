package aerys.minko.render
{
	public interface IRenderer3DDecorator extends IRenderer3D
	{
		function get target() : IRenderer3D;
	}
}