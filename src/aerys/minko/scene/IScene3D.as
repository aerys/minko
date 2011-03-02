package aerys.minko.scene
{
	import aerys.minko.query.IScene3DQuery;

	public interface IScene3D
	{
		function accept(query : IScene3DQuery) : void;
		
		function get name() : String;
	}
}