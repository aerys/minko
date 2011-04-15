package aerys.minko.scene.visitor.action
{
	import aerys.minko.scene.graph.IScene;

	public interface IVisitorAction
	{
		function get name() : String;
		
		function enter(scene : IScene) : Boolean;
		function visit(scene : IScene) : Boolean;
		function leave(scene : IScene) : Boolean;
	}
}