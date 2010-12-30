package aerys.minko.scene
{
	import aerys.minko.render.visitor.IScene3DVisitor;
	import aerys.minko.scene.group.IGroup3D;

	public interface IScene3D
	{
		function visited(visitor : IScene3DVisitor) : void;
		
		function get name() : String;
	}
}