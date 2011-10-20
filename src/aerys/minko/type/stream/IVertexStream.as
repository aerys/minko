package aerys.minko.type.stream
{
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	public interface IVertexStream extends IVersionable
	{
		function get format() 			: VertexFormat;
		function get length()			: uint;
		function get isDynamic()			: Boolean;

		function deleteVertexByIndex(index : uint) : Boolean;
		function getSubStreamByComponent(component : VertexComponent) : VertexStream;
	}
}