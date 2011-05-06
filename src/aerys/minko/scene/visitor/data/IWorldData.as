package aerys.minko.scene.visitor.data
{

	public interface IWorldData
	{
		function setLocalDataProvider(styleStack	: StyleStack, 
									  transformData	: TransformData) : void;
		
		function invalidate() : void;
		
		function reset() : void;
	}
}