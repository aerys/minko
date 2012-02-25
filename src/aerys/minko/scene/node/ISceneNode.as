package aerys.minko.scene.node
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.Signal;

	public interface ISceneNode
	{
		function get root() : ISceneNode;
		function get parent() : Group;
		function set parent(value : Group) : void;
		
		function get name() : String;
		function set name(value : String) : void;
		
		function get added() : Signal;
		function get removed() : Signal;
	}
}