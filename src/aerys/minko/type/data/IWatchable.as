package aerys.minko.type.data
{
	import aerys.minko.type.Signal;

	public interface IWatchable
	{
		function get changed() : Signal;
	}
}
