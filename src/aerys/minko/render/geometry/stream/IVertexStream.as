package aerys.minko.render.geometry.stream
{
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.type.Signal;

	public interface IVertexStream
	{
		function get format() 			: VertexFormat;
		function get numVertices()		: uint;
		
		function get changed()			: Signal;
		function get boundsChanged()	: Signal;
		
		function getVertexProperty(index 		: uint,
								   component 	: VertexComponent 	= null,
								   offset 		: uint 				= 0) : Number;
		
		function setVertexProperty(index 		: uint,
								   value 		: Number,
								   component 	: VertexComponent 	= null,
								   offset	 	: uint 				= 0) : void;
		

		function deleteVertex(index : uint) : IVertexStream;
		function duplicateVertex(index : uint) : IVertexStream;
		
		function disposeLocalData(waitForUpload : Boolean = true) : void;
		function getStreamByComponent(component : VertexComponent) : VertexStream;
		function dispose() : void;
	}
}