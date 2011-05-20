package aerys.minko.scene.node.group
{
	import aerys.minko.scene.node.IScene;
	
	public interface IGroup extends IScene
	{
		function get numChildren() : uint;
		
		function addChild(scene : IScene) : IGroup;
		function addChildAt(scene : IScene, position : uint) : IGroup;
		
		function removeChild(child : IScene) : IGroup;
		function removeChildAt(position : uint) : IGroup;
		function removeAllChildren() : IGroup;
		
		function contains(scene : IScene) : Boolean
			
		function swapChildren(child1 : IScene, child2 : IScene) : IGroup;
		
		function getChildAt(position : uint) : IScene;
		function getChildIndex(child : IScene) : int;
		function getChildByName(name : String) : IScene;
		
		function getDescendantByName(name : String) : IScene;
	}
}