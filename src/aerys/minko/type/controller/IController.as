package aerys.minko.type.controller
{
	import aerys.minko.scene.node.IScene;

	public interface IController
	{
		function get targets() : Vector.<IScene>;
	}
}