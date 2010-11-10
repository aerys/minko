package aerys.minko.scene.group
{
	import aerys.minko.scene.IScene3D;
	
	public interface IGroup3D extends IScene3D
	{
		function get numChildren() : uint;
		
		function addChild(myObject : IScene3D) : IScene3D;
		function addChildAt(myObject : IScene3D, myPosition : uint) : IScene3D;
		
		function removeChild(myChild : IScene3D) : IScene3D;
		function removeChildAt(myPosition : uint) : IScene3D;
		function removeAllChildren() : uint;
		
		function contains(myChild : IScene3D) : Boolean
			
		function swapChildren(myChild1 : IScene3D, myChild2 : IScene3D) : Boolean;
		
		function getChildAt(myPosition : uint) : IScene3D;
		function getChildIndex(myChild : IScene3D) : int;
		function getChildByName(myName : String) : IScene3D;
	}
}