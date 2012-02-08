package aerys.minko.type.controller
{
	import aerys.minko.scene.ISceneNode;

	public interface IController
	{
		function get targets() : Vector.<ISceneNode>;
	}
}