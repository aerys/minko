package aerys.minko.type.stream
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	public interface IVertexStream
	{
		function get format() 	: VertexFormat;
		function get length()	: uint;
		
		function get changed()			: Signal;
		function get boundsChanged()	: Signal;

		function disposeLocalData(waitForUpload : Boolean = true) : void;
		function deleteVertexByIndex(index : uint) : Boolean;
		function getStreamByComponent(component : VertexComponent) : VertexStream;
		function dispose() : void;
	}
}