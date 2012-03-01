package aerys.minko.type.loader
{
	import aerys.minko.type.Signal;
	
	import flash.net.URLRequest;
	import flash.utils.ByteArray;

	public interface ILoader
	{
		function get progress()		: Signal;
		function get error()		: Signal;
		function get complete()		: Signal;
		
		function get isComplete()	: Boolean;
		
		function load(urlRequest : URLRequest) : void;
		function loadClass(classObject : Class) : void;
		function loadBytes(data : ByteArray) : void;
	}
}
