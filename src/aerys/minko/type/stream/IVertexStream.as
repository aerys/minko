package aerys.minko.type.stream
{
	import aerys.minko.render.resource.VertexBufferResource;
	import aerys.minko.type.IVersionnable;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	public interface IVertexStream extends IVersionnable
	{
		function get format() 			: VertexFormat;
		function get length()			: uint;
		function get dynamic()			: Boolean;
		
		function deleteVertexByIndex(index : uint) : Boolean;
		function getSubStreamByComponent(component : VertexComponent) : VertexStream;
		function getSubStreamById(id : int) : VertexStream;
	}
}