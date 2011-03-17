package aerys.minko.scene.group
{
	import aerys.minko.scene.IScene3D;
	
	public interface IGroup3D extends IScene3D
	{
		function get numChildren() : uint;
		
		function addChild(scene : IScene3D) : IScene3D;
		function addChildAt(scene : IScene3D, myPosition : uint) : IScene3D;
		
		function removeChild(child : IScene3D) : IScene3D;
		function removeChildAt(position : uint) : IScene3D;
		function removeAllChildren() : uint;
		
		function contains(scene : IScene3D) : Boolean
			
		function swapChildren(child1 : IScene3D, child2 : IScene3D) : Boolean;
		
		function getChildAt(position : uint) : IScene3D;
		function getChildIndex(child : IScene3D) : int;
		function getChildByName(name : String) : IScene3D;
		
		function getDescendantByName(name : String) : IScene3D;
	}
}