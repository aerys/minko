package aerys.minko.scene.mesh.parser
{
	import aerys.minko.scene.mesh.IMesh3D;
	
	import flash.utils.ByteArray;
	
	public interface IMeshParser
	{
		function get meshes() : Vector.<IMesh3D>;
		
		function parse(myData : ByteArray) : void;
	}
}