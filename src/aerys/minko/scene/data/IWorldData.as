package aerys.minko.scene.data
{
	import flash.utils.Dictionary;

	public interface IWorldData
	{
		function setDataProvider(styleStack	: StyleData, 
								 transformData	: TransformData,
								 worldData	: Dictionary) : void
		
		function invalidate() : void;
		
		function reset() : void;
	}
}