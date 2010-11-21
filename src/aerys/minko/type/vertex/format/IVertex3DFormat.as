package aerys.minko.type.vertex.format
{
	public interface IVertex3DFormat
	{
		function get offsets() 			: Object;
		function get dwordsPerVertex() 	: int;
		function get nativeFormats()	: Vector.<int>;
	}
}