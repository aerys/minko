package aerys.minko.scene.interfaces
{
	import aerys.minko.query.renderdata.world.IWorldData;
	import aerys.minko.query.renderdata.transform.TransformManager;

	public interface IWorldObject
	{
		function get isSingle()	: Boolean;
		
		function getData(tm : TransformManager) : IWorldData;
	}
}