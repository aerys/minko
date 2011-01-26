package aerys.minko.data.parser
{
	import aerys.minko.scene.IScene3D;
	
	import flash.utils.ByteArray;

	public interface IParser3D
	{
		function get data() : Vector.<IScene3D>;
		
		function parse(data : ByteArray) : Boolean
	}
}