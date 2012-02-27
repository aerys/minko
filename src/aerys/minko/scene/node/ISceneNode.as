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
		
		function getController(index : uint) : AbstractController;
		function addController(controller : AbstractController) : void;
		function removeController(controller : AbstractController) : void;
		
		function get numControllers() : uint;
		function get controllerAdded() : Signal;
		function get controllerRemoved() : Signal;
	}
}