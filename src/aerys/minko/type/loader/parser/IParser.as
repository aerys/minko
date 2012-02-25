package aerys.minko.type.loader.parser
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.loader.ILoader;
	
	import flash.utils.ByteArray;

	public interface IParser
	{
		function get error() : Signal;
		function get progress()	: Signal;
		function get complete() : Signal;
		
		function isParsable(data : ByteArray) : Boolean;
		function getDependencies(data : ByteArray) : Vector.<ILoader>;
		function parse() : void;
	}
}
