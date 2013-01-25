package aerys.minko.type.binding
{
	import aerys.minko.type.Signal;

	public interface IDynamicDataProvider extends IDataProvider
	{
		function get propertyAdded() 	: Signal;
		function get propertyRemoved() 	: Signal;
	}
}