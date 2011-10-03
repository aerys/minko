package aerys.minko.type.parser
{
	import aerys.minko.scene.node.IScene;

	import flash.events.IEventDispatcher;
	import flash.utils.ByteArray;

	public interface IParser extends IEventDispatcher
	{
		function get data() : Vector.<IScene>;

		function parse(data : ByteArray, options : ParserOptions) : Boolean;
	}
}