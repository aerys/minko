package aerys.minko.type.controller
{
	import aerys.minko.scene.node.ISceneNode;

	public interface IController
	{
		function get targets() : Vector.<ISceneNode>;
	}
}