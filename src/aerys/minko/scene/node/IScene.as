package aerys.minko.scene.node
{
	import aerys.minko.scene.Visitor;
	import aerys.minko.type.Signal;

	public interface IScene
	{
		function get parent() : Group;
		function set parent(value : Group) : void;
		
		function get name() : String;
		function set name(value : String) : void;
		
		function get added() : Signal;
		function get removed() : Signal;
	}
}