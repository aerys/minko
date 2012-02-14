package aerys.minko.type.data
{
	import aerys.minko.type.Signal;

	public interface IDataProvider
	{
		function get dataDescriptor() : Object;
		
		function get changed() : Signal;
		
		function get locked() : Boolean;
		function lock() : void;
		function unlock() : void;
	}
}