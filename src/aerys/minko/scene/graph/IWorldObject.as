package aerys.minko.scene.graph
{
	import aerys.minko.scene.visitor.data.IWorldData;
	import aerys.minko.scene.visitor.data.TransformManager;

	public interface IWorldObject
	{
		function get isSingle()	: Boolean;
		
		function getData(tm : TransformManager) : IWorldData;
	}
}