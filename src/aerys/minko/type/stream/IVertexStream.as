package aerys.minko.type.stream
{
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	public interface IVertexStream
	{
		function get format() 	: VertexFormat;
		function get length()	: uint;

		function disposeLocalData(waitForUpload : Boolean = true) : void;
		function deleteVertexByIndex(index : uint) : Boolean;
		function getStreamByComponent(component : VertexComponent) : VertexStream;
		function dispose() : void;
	}
}