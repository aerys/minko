package aerys.minko.type.stream
{
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;

	public interface IVertexStream
	{
		function get format() : VertexFormat;
		function get length() : uint;
		
		function deleteVertexByIndex(index : int) : Boolean;
		function getVertexStreamByComponent(vertexComponent : VertexComponent) : VertexStream;
	}
}