package aerys.minko.scene.visitor.data
{

	public interface IWorldData
	{
		function setLocalDataProvider(styleStack	: StyleStack, 
									  localData		: LocalData) : void;
		
		function invalidate() : void;
		
		function reset() : void;
	}
}