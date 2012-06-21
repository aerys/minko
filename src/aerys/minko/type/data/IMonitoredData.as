package aerys.minko.type.data
{
	import aerys.minko.type.Signal;

	public interface IMonitoredData
	{
		function get changed()	: Signal;
	}
}
