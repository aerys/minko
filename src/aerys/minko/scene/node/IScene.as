package aerys.minko.scene.node
{
	import aerys.minko.scene.visitor.ISceneVisitor;

	public interface IScene
	{
		function visited(query : ISceneVisitor) : void;
		
		function get name() : String;
	}
}