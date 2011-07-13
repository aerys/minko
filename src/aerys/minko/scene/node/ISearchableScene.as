package aerys.minko.scene.node
{
	public interface ISearchableScene extends IScene
	{
		function getDescendantByName(name : String) : IScene;
	}
}