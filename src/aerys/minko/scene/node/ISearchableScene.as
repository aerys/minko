package aerys.minko.scene.node
{
	public interface ISearchableScene extends IScene
	{
		function getDescendantByName(name : String) : IScene;
		function getDescendantsByType(type : Class, descendants : Vector.<IScene> = null) : Vector.<IScene>;
	}
}