package aerys.minko.type.binding
{
	import aerys.minko.type.Signal;

	public interface IWatchable
	{
		function get changed() : Signal;
	}
}
