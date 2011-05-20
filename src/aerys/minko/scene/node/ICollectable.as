package aerys.minko.scene.node
{
	import aerys.minko.scene.visitor.Visitor;

	public interface ICollectable extends IScene
	{
		function collected(query : Visitor) : IScene;
	}
}
