package aerys.minko.type.data
{
	import aerys.minko.type.Signal;

	public interface IDynamicDataProvider extends IDataProvider
	{
		function get propertyAdded() : Signal;
		function get propertyRemoved() : Signal;
	}
}