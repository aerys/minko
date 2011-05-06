package aerys.minko.query.renderdata.world
{
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;

	public interface IWorldData
	{
		function setLocalDataProvider(styleStack	: StyleStack3D, 
									  transformData	: TransformData) : void;
		
		function invalidate() : void;
		
		function reset() : void;
	}
}