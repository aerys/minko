package aerys.minko.type.data
{
	import aerys.minko.type.Signal;

	public interface IDataProvider extends IMonitoredData
	{
		function get dataDescriptor() 	: Object;
		function get usage()			: uint;
		
		function clone()				: IDataProvider;
	}
}
