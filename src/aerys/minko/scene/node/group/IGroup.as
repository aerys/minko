package aerys.minko.scene.node.group
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ISearchableScene;
	
	public interface IGroup extends ISearchableScene
	{
		function get numChildren() : uint;
		
		function addChild(scene : IScene) : IGroup;
		function addChildAt(scene : IScene, position : uint) : IGroup;
		
		function removeChild(child : IScene) : IGroup;
		function removeChildAt(position : uint) : IGroup;
		function removeAllChildren() : IGroup;
		
		function contains(scene : IScene) : Boolean
			
		function getChildAt(position : uint) : IScene;
		function getChildIndex(child : IScene) : int;
		function getChildByName(name : String) : IScene;
	}
}