package aerys.minko.type.binding
{
	import aerys.minko.type.Signal;

	public interface IDataProvider extends IWatchable
	{
		function get dataDescriptor() 	: Object;
		function get usage()			: uint;
		function get propertyChanged()	: Signal;
		
		function clone()				: IDataProvider;
	}
}
