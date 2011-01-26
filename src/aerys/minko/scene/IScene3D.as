package aerys.minko.scene
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.group.IGroup3D;

	public interface IScene3D
	{
		function accept(query : IScene3DQuery) : void;
		
		function get name() : String;
	}
}