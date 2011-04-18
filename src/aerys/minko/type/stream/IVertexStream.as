package aerys.minko.type.stream
{
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;

	public interface IVertexStream
	{
		function get format() : VertexFormat;
		function get length() : int;
		
		function deleteVertexByIndex(index : int) : Boolean;
		function getStreamByComponent(vertexComponent : VertexComponent) : VertexStream;
	}
}