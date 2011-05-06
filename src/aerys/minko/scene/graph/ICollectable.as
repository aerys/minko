package aerys.minko.scene.graph
{
	import aerys.minko.scene.visitor.Visitor;

	public interface ICollectable extends IScene
	{
		function collected(query : Visitor) : IScene;
	}
}
